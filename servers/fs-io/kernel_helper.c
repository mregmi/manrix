/**
 * FS and I/O manager for XXX OS
 * Copyright (C) 2004, 2005	Manish Regmi (regmi dot manish at gmail dot com)
 *				            Rajesh R.C (rajee5283 at hotmail dot com)
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
 **/
/**
 * File: kernel_helper.c
 *
 * We implement helper routines for kernel here.
 **/
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <mrx/fd.h>
#include <asm/syscall.h>
#include <asm/ipc.h>
#include <asm/vm.h>
#include <mrx/pager.h>

#include "vfs.h"
#include "elf.h"

/* Exec: */
int do_exec(tid_t tid, char *path, struct vn_object *vno)
{
	int fdesc = -1, eaccess = O_RDONLY;
//	struct userid id;
	struct file file;
	struct vnode *vnode;
	struct vattr attr;
//	printf("NAMEV %s ", path);
	vnode = vfs_namev(tid, path, &eaccess, 0);
	if(!vnode)
		return -1;

	vgetattr(vnode, &attr);

	/* Is it a regular file? */
	if(!S_ISREG(attr.st_mode))
	{
		eaccess = -EINVAL;
		goto err;
	}

	/* Do we have execute Permission */
	if(!CAN_EXEC(&id, attr.st_mode))
	{
		eaccess = -EPERM;
		goto err;
	}


	eaccess = open_elf(vnode, vno);
	if(eaccess < 0)
	{
		DEBUG("Open_elf failed with %d ", eaccess);
		goto err;
	}
	file.f_ref = 0;
	file.f_access = eaccess;
	file.f_offset = 0;
	file.vnode.inode = vnode->v_inum;
	file.vnode.dev = vnode->v_rdev;

	vno->fd = syscall_fd_alloc(tid, fdesc, &file);
	vno->blk_size = vnode->vfsp->blk_size;
	vno->map_type = MAP_FIXED;
	vno->size = attr.st_size;
	vno->pager_id = ID_FSIO_MNGR;

	pthread_mutex_unlock(&vnode->smlock);
	return 0;

err:
	vput(vnode);
	pthread_mutex_unlock(&vnode->smlock);
	return eaccess;
}

/*
 * read the data of the memory mapped file.
 */
static inline int read_file_data(struct vnode *vnode, struct vn_object *vn, void *data)
{
	ssize_t size;
	unsigned long block, off;
	struct bcache *buffer;
	char *ptr = (char *)data;
	
	if(vn->blk_size >= vn->page_size)
		size = vn->blk_size;
	else
		size = vn->page_size;
//	printf("size is %d ps %d bs %d ", size, vn->page_size, vn->blk_size);
	off = vn->offset/vnode->vfsp->blk_size;
	vn->offset += size;
	size = size/vnode->vfsp->blk_size;
	do{
		block = vnode->ops->vop_bmap(vnode, off);
//		printf("Block is %d off is %d ", block, off);
		buffer = bread(vnode->v_rdev, block, vnode->vfsp->blk_size);
		if(!buffer)
			return -1;
		
		memcpy(ptr, buffer->b_data, vnode->vfsp->blk_size);
		ptr = (void *)((unsigned long)ptr + vnode->vfsp->blk_size);
		brelse(buffer);
		off++;
		
	}while(--size);
	
	return 0;	
}
 
/* Data request:
 * 	Request data, used by pager
 */
int data_request(tid_t tid, struct vn_object *vn, void *data)
{
	struct vnode *vnode;
	struct file file;
	int err;
//	printf("jsfsf  ");
	if(!vn)
		return -1;
//	printf("234 ");
	err = get_openfile(tid, vn->fd, &file);
	if(err < 0)
		return err;
//	printf("requesting from %X size %d \n", vn->va, vn->blk_size);
	vnode = find_vnode(file.vnode.dev, file.vnode.inode);

	if(!vnode)
		return -1;
	
	if(vn->map_type & VM_MAP_FILE)
		return read_file_data(vnode, vn, data);
	else	
		return elf_read_data(vnode, vn, data);
}

/* Get the current working directory */
char *fs_getcwd(tid_t tid, char *ptr)
{
	if(!ptr)
		return NULL;

	return NULL;
}

/* Change the current Working Directory */
int fs_chdir(tid_t tid, char *path)
{
	int eaccess = O_RDONLY, len;
	struct vnode *vnode;
	char ptr[PATH_MAX], *ans = NULL;
	struct indev ind;

	vnode = vfs_namev(tid, path, &eaccess, 0);

	if(vnode == NULL)
		return eaccess;
	
	if(!S_ISDIR(vnode->v_type))
		return -ENOTDIR;
	
	pthread_mutex_unlock(&vnode->smlock);
	syscall_get_cwd(tid, NULL, ptr);
	
	if(path[0] == '/')
	{
		ans = path;
		goto end;
	}
	len = strlen(ptr);
	strcpy(ptr, "/");
	if(path[0] == '.' && path[1] != '.')
		path++;
	strcpy(ptr, path);
	ans = &ptr;	
end:
	ind.inode = vnode->v_inum;
	ind.dev = vnode->v_rdev;
	syscall_put_cwd(tid, &ind, ans);

	return 0;
}
