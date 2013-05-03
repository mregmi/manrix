/*
 * FS and I/O manager for XXX OS
 * Copyright (C) 2004, 2005	Manish Regmi (regmi dot manish at gmail dot com)
 *				Rajesh R.C (rajee5283 at hotmail dot com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/*
 * File: file.c
 * 		The Basic File Operations independent of File System.
 */

#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>

#include <mrx/fd.h>
#include <asm/syscall.h>

#include "vfs.h"
#include "elf.h"

/* File Systems Supported */
extern struct vfssw *fs;

/* fs lock */
extern pthread_spinlock_t fslock;

/* Pointer to the root file system */
struct vfs *rootvfs = NULL;

/* List Of file systems mounted*/
struct linked_list mnted = {NULL, NULL};

/* Lock for mntedlist. */
pthread_spinlock_t mntlock = SPIN_LOCK_UNLOCKED;

/* get the file attributes */
int vgetattr(struct vnode *vn, struct vattr *attr)
{
	if(!vn->ops && !vn->ops->vop_get_attr)
		return -1;

	vn->ops->vop_get_attr(vn, attr);
	return 0;
}

/* store the file attributes */
int vputattr(struct vnode *vn, struct vattr *attr)
{
	if(!vn->ops && !vn->ops->vop_get_attr)
		return -1;
	vn->ops->vop_put_attr(vn, attr);
	return 0;
}

/* determine accessibility of a file */
int vfs_access(tid_t tid, char *path, int amode)
{
	int eaccess;
	struct vnode *vnode;

	if((amode & R_OK) || (amode & X_OK))
		eaccess |= O_RDONLY;
	if(amode & W_OK)
		eaccess |= O_WRONLY;

	vnode = vfs_namev(tid, path, &eaccess, 0);

	if(vnode == NULL)
			return eaccess;

	pthread_mutex_unlock(&vnode->smlock);
	return 0;
}


/* open a file */
int vfs_open(tid_t tid, char *path, int *access, mode_t mode)
{
	int fdesc = -1, eaccess = *access;
	struct file file;
	struct vnode *vnode;

	printf("Opening File %s.\n", path);
//	sched_yield();
	vnode = vfs_namev(tid, path, &eaccess, mode);
//	printf("eaccess %x, vnode %d ", eaccess, vnode->v_inum);

	if(vnode == NULL)
			return eaccess;

	file.f_ref = 0;
	file.f_access = *access;
	file.f_offset = 0;
	file.vnode.inode = vnode->v_inum;
	file.vnode.dev = vnode->v_rdev;
	if(vnode->ops && vnode->ops->vop_open)
		eaccess = vnode->ops->vop_open(&file, vnode, mode);
//	printf("open is %X ", vnode->ops->vop_open);
//	for(;;);

	*access = vnode->v_type;

	/* namev returns locked vnode */
	pthread_mutex_unlock(&vnode->smlock);
	if(eaccess < 0)
		return eaccess;

	fdesc = syscall_fd_alloc(tid, fdesc, &file);
//	printf("allocating eaccess %X fdesc %d %s", eaccess, fdesc);
	return FDESC_CREATE(eaccess, fdesc);
}

/* close a file */
int vfs_close(tid_t tid, int fdesc)
{
	struct file file;
	struct vnode *vn;
	int ret;

	ret = syscall_fd_free(tid, FDESC_FD(fdesc), &file);
	if(ret < 0)
		return ret;

	vn = find_vnode(file.vnode.inode, file.vnode.dev);
	if(!vn)
		return -EINVAL;

	vput(vn);

	return ret;
}

/* Read from a file */
ssize_t vfs_read(tid_t tid, int fdesc, void *buf, size_t nsize)
{
	struct file file;
	struct vnode *vnode;
	int err = -EBADF;

	err = get_openfile(tid, fdesc, &file);
//	printf(" read request: nsize %d fd %d ", nsize,  fdesc);
	vnode = find_vnode(file.vnode.dev, file.vnode.inode);
	if(!vnode)
		goto errn;
//	printf(" vnode found ");
	pthread_mutex_lock(&vnode->smlock);
	if(!vnode->ops && !vnode->ops->vop_read)
		return -ENODEV;

	err = vnode->ops->vop_read(&file, vnode, buf, nsize);
	if(err < 0)
		goto errn;
//	printf(" read over ");
	put_openfile(tid, fdesc, &file);
//	printf(" openfile done ");
//	pthread_mutex_unlock(&vnode->smlock);
errn:

//	printf(" reading done %s ", buf);

	return err;
}

/* Write to a file */
ssize_t vfs_write(tid_t tid, int fdesc, void *buf, size_t nsize)
{
	struct file file;
	struct vnode *vnode;
	int err = -EBADF;

	err = get_openfile(tid, fdesc, &file);

	if(!(file.f_access & O_WRONLY))
		return -EPERM;

	vnode = find_vnode(file.vnode.inode, file.vnode.dev);
	if(!vnode)
		goto errn;

	if(vnode->ops && vnode->ops->vop_write)
		err = vnode->ops->vop_write(&file, vnode, buf, nsize);

	put_openfile(tid, fdesc, &file);
errn:
	return err;
}

/* Mount a File System */
int vfs_mount(tid_t tid, char *spec, char *dir,
	int flags, char *name, void *data, size_t datalen)
{
	struct vfs *vfs;
	struct vfssw *vsw;
	struct vnode *device = NULL;
	int index = 0, err = -EINVAL;

	/////FIXME:- Check for permission
//	if(!superuser(tid))
//		return -EPERM;

	pthread_spin_lock(&fslock);
	do{
		vsw = &fs[index];
		if(strcmp(name, vsw->vsw_name) == 0)
			break;
		index++;
	}while(vsw->vsw_init != NULL);
	pthread_spin_unlock(&fslock);

	vfs = vsw->vsw_init();
	pthread_mutex_lock(&vfs->lock);

	vfs->vfs_mpoint = vfs_namev(tid, dir, &err, 0);
	if(vfs->vfs_mpoint == NULL)
		goto freevfs;
	if(vfs->vfs_mpoint->v_type != VNODE_DIR)
		goto unlocknfreemnt;

	device = vfs_namev(tid, spec, &err, 0);
	vfs->vfs_dev = device->v_rdev;

	if(!device)
		goto unlocknfreemnt;
	if(device->v_type != VNODE_BLK)
		goto unlocknfreeboth;

	err = vfs->ops->vfs_mount(vfs, flags);
	if(err < 0)
		goto unlocknfreeboth;

	pthread_spin_lock(&mntlock);
	list_add(&mnted, vfs, vfs_next, struct vfs *);
	pthread_spin_unlock(&mntlock);

	pthread_mutex_unlock(&vfs->vfs_mpoint->smlock);
	pthread_mutex_unlock(&device->smlock);
	pthread_mutex_unlock(&vfs->lock);

	return 0;
unlocknfreeboth:
	pthread_mutex_unlock(&vfs->vfs_mpoint->smlock);
unlocknfreemnt:
	pthread_mutex_unlock(&device->smlock);
freevfs:
	pthread_mutex_unlock(&vfs->lock);
	free(vfs);
	return err;
}

/* UnMount a File System */
int vfs_umount(tid_t tid, char *target)
{
	struct vfs *vfs = NULL;
//	struct vfssw *vsw;
	struct vnode *mpoint;
	int err = 0;

	/////FIXME:- Check for permission
//	if(!superuser(tid))
//		return -EPERM;

	mpoint = vfs_namev(tid, target, &err, 0);
	if(!mpoint)
		goto freevfs;
	if(mpoint->v_type != VNODE_DIR)
		goto unlocknfree;

	vfs = mpoint->v_fsmounted;
	if(!vfs)
		goto unlocknfree;

	err = vfs->ops->vfs_unmount(vfs);
	if(err < 0)
		goto unlocknfree;

	pthread_spin_lock(&mntlock);
	list_remove(&mnted, vfs, vfs_next, struct vfs *);
	pthread_spin_unlock(&mntlock);

	free(vfs);
	return 0;
unlocknfree:
		pthread_mutex_unlock(&mpoint->smlock);
freevfs:
	free(vfs);
	return err;
}

/* Change the protection mode of the file */
int vfs_chmod(tid_t tid, char *path, mode_t mode)
{
	struct vnode *vn;
	int err = O_RDWR;
	struct vattr attr;

	/////FIXME:- Check for permission

	vn = vfs_namev(tid, path, &err, 0);
	if(!vn)
		goto errn;

	vgetattr(vn, &attr);

	attr.st_mode = mode;
	vputattr(vn, &attr);
	pthread_mutex_unlock(&vn->smlock);

	/* Release the last vnode */
	vput(vn);

errn:
	return err;
}

/* get the file status */
int vfs_stat(tid_t tid, char *path, struct vattr *stat)
{
	struct vnode *vn;
	int err = O_RDONLY;

	/////FIXME:- Check for permission
printf("going to vattr ");
	for(;;) sched_yield();
	vn = vfs_namev(tid, path, &err, 0);
	if(!vn)
		goto errn;
	
	sched_yield();
	vgetattr(vn, stat);
	printf("Attribute Success ");
	sched_yield();
	pthread_mutex_unlock(&vn->smlock);

	/* Release the last vnode */
	//vput(vn);

errn:
	printf("end ");
	return 0;
}

/* get the file status */
int vfs_fstat(tid_t tid, int fdesc, struct vattr *stat)
{
	struct file file;
	struct vnode *vn;
	int err = O_RDWR;
printf("going to vattr ");
	for(;;) sched_yield();
	/////FIXME:- Check for permission
	err = get_openfile(tid, fdesc, &file);
	if(err < 0)
		return -ESRCH;

	vn = find_vnode(file.vnode.dev, file.vnode.inode);
	if(!vn)
		goto errn;

	pthread_mutex_lock(&vn->smlock);

	vgetattr(vn, stat);

	pthread_mutex_unlock(&vn->smlock);

errn:
	return err;
}

/* get the file status */
int vfs_fchmod(tid_t tid, int fdesc, mode_t mode)
{
	struct file file;
	struct vnode *vn;
	struct vattr attr;
	int err = O_RDWR;

	/////FIXME:- Check for permission
	err = get_openfile(tid, fdesc, &file);
	if(err < 0)
		return -ESRCH;

	vn = find_vnode(file.vnode.inode, file.vnode.dev);
	if(!vn)
		goto errn;

	pthread_mutex_lock(&vn->smlock);

	vgetattr(vn, &attr);

	attr.st_mode = mode;
	vputattr(vn, &attr);
	pthread_mutex_unlock(&vn->smlock);

	/* Release the last vnode */
	vput(vn);

errn:
	return err;
}

int vfs_chown(tid_t tid, char *path, uid_t uid, gid_t gid)
{
	struct vnode *vn;
	int err = O_RDWR;
	struct vattr attr;

	vn = vfs_namev(tid, path, &err, 0);
	if(vn == NULL)
		goto errn;

	vgetattr(vn, &attr);

	if(uid != -1)
		attr.st_uid = uid;
	if(gid != -1)
		attr.st_gid = gid;

	vputattr(vn, &attr);
	pthread_mutex_unlock(&vn->smlock);

	/* Release the last vnode */
	vput(vn);

errn:
	return err;
}

