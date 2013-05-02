/*
 * FS and I/O manager for ManRiX OS
 * Copyright (C) 2004, 2005 Manish Regmi (regmi dot manish at gmail dot com)
 *	Rajesh Bikram R.C   (rajee5283 at hotmail dot com)
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
 *
 *
 * File: vfs.h
 */

#ifndef __VFS_H
#define __VFS_H

#include <sys/types.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include <mrx/fd.h>
#include <mrx/lock.h>
#include <mrx/list.h>
#include <mrx/atomic.h>

#include "bcache.h"
#include "ext2fs.h"
#include "fatfs.h"
#include "svrconfig.h"

#define MAX_DEF_FD		32
#define FD_HASH_SZ		4096
#define VNODE_HASH_SZ	4096

/* Maximum no of File Systems Supported */
#define MAX_DEF_FS		20

/* Vnode v_flag values */
#define V_ROOT			1
#define V_FREE			2
#define V_LOCKED		4
#define V_DIRTY			8
#define V_NEW			16

/* Vnode File Types */
#define VNODE_FIFO		0x1
#define VNODE_CHR		0x2
#define VNODE_DIR		0x4
#define VNODE_BLK		0x6
#define VNODE_REG		0x8
#define VNODE_LNK		0xA
#define VNODE_SOCK		0xC

/* ManRiX use only, file access mode  */
#define O_SYMLINK	0x1000

#ifdef FSIO_DEBUG
 static inline void DEBUG(const char *string, ...)
 {
 	char buf[512];
	va_list ap;
	
	va_start(ap, string);
	vsprintf(buf, string, ap);
 	printf("%s: %d: %s", __FILE__, __LINE__, buf);
	va_end(ap);
 }
 
#else
 #define DEBUG		do{} while(0); 

#endif	/* FSIO_DEBUG */
 
/* the vfs object */
struct vfs{
	struct linked_list vfs_next;	/* vfs chain */
	struct vnode *vfs_mpoint;	/* Vnode of mount point */

	dev_t		vfs_dev;
	size_t		blk_size;		/* Size of Block in this Vfs */

	/* fs specfic structure e.g. superblock */
	union {
		struct ext2sb ext2_sb;
		struct fat_super_block	fat;
	}sb;
#define ext2sb	ext2_sb.ext2
#define ext2gdesc ext2_sb.gdes
#define fatsb	sb.fat

	struct vfsops *ops;
	pthread_mutex_t lock;
};


/* the vnode object */
struct vnode{
	unsigned short	v_flag;		/* vnode flag */
	atomic_t	v_count;	/* reference count */
	struct vfs 		*v_fsmounted; /* for mount mount point */
	struct vfs		*vfsp;		/* FS i belong to */

	ino_t			v_inum;		/* Inode Number (Unique to each FS) */
	dev_t			v_rdev;		/* device id */
	int 			v_type;		/* File type */

	struct linked_list v_freelist;	/* Free vnode list */
	struct vnode	*v_next;	/* hash table next pointer */
	struct vnodeops	*ops;
	void *data;			/* For special purpose of exe'd file */
	pthread_mutex_t smlock;		/* For blocking ops */
	/* FS specfic data */
	union inode{
		struct ext2inode	ext2inode;
//		struct fat_inode	fatinode;
	} v_ino;
};

struct vnodehash{
	struct vnode *vn;
};


struct procdata{
	struct file file;
	struct userid id;
	struct vnode *vnode;
	struct vnode *cwd;
};


/* Vfs operations */
struct vfsops{
	int (* vfs_mount)();
	int (* vfs_unmount)();
	int (* vfs_statvfs)();
	int (* vfs_sync)();
};

struct nameidata{
	struct vnode *vn;
	struct userid id;
	struct vfs *vfs;
	char *path;	/* pointer to path if mountpoint reached */
	mode_t mode;
	int access;
};


/*
 * File System independent File Attributes
 * Some FS may not support all the attributes.
 */
struct vattr{
	dev_t     st_dev;     /* Device ID of device containing file. */
	ino_t     st_ino;     /* File serial number. */
	mode_t    st_mode;    /* Mode of file (see below). */
	nlink_t   st_nlink;   /* Number of hard links to the file. */
	uid_t     st_uid;     /* User ID of file. */
	gid_t     st_gid;     /* Group ID of file. */
	dev_t     st_rdev;    /* Device ID (if file is character or block).*/

	/*
	 * For regular files, the file size in bytes.
	 * For symbolic links, the length in bytes of the
	 * pathname contained in the symbolic link.
	 */
	off_t     st_size;
	time_t    st_atime;		/* Time of last access.*/
	time_t    st_mtime;		/* Time of last data modification. */
	time_t    st_ctime;		/* Time of last status change. */

	/*
	 * A file system-specific preferred I/O block size for
	 * this object. In some file system types, this may
	 * vary from file to file.
	 */
	blksize_t st_blksize;
	blkcnt_t  st_blocks;  	/* Number of blocks allocated for this object.*/
};


/* Vnode Operations */
struct vnodeops{
	int (* vop_open)(struct file *, struct vnode *, mode_t);
	int (* vop_close)(struct file *, struct vnode *);
	int (* vop_read)(struct file *, struct vnode *, void *, size_t);
	int (* vop_write)(struct file *, struct vnode *, void *, size_t);
	int (* vop_strategy)();
	int (* vop_ioctl)();
	int (* vop_create)();
	int (* vop_remove)();
	int (* vop_link)();
	int (* vop_unlink)();
	int (* vop_rename)();
	int (* vop_mkdir)();
	int (* vop_rmdir)();
	int (* vop_readdir)();
	int (* vop_symlink)();
	int (* vop_poll)();

	/* Internal use only */
	int (* vop_read_inode)(struct vnode *, ino_t);
	unsigned long (* vop_bmap)(struct vnode *, unsigned long);
	void (* vop_write_inode)(struct vnode *);
	int (* vop_namei)(struct vnode *, char *, struct nameidata *);
	void (* vop_get_attr)(struct vnode *, struct vattr *);
	void (* vop_put_attr)(struct vnode *, struct vattr *);
	int  (* vop_req_iospace)(int , tid_t);
};


/* Virtual File System Switch */
struct vfssw{
	char	*vsw_name;
	struct vfs *(* vsw_init)();
	struct vfsops *vswops;
};

typedef int (* init_t)();
/* Initializations for vfs */
struct vfsinit {
	int (* init)();
	int (* exit)();
};

#define CAN_EXEC(id, mode)	1

/*
 * Find vnode by its device number and block number
 * The hash table must be locked.
 */
extern struct vnode *find_vnode(dev_t ,ino_t );

/* do not support this action */
static inline int	nodev()
{
	return -ENODEV;
}

/* No special action associated with this entry */
static inline int nulldev()
{
	return 0;
}

static inline int superuser(struct userid *id)
{
	if(id->euid == 0)
		return 1;
	return 0;
}

#include <asm/syscall.h>

static inline struct vnode *get_cwd(tid_t tid)
{
	struct indev indev;

	syscall_get_cwd(tid, &indev, NULL);

	return find_vnode(indev.inode, indev.dev);
}


/*
 * File System independent Name to vnode
 * the access value returns error code.
 */
extern struct vnode *vfs_namev(tid_t, char *,
			int *, mode_t );

/* Allocate a vnode but first search in the cache. */
extern struct vnode *vget(struct vnode *, ino_t , dev_t );

/* Release a Vnode */
extern void vput(struct vnode *);

/* initialize VFS */
extern void vfs_init(const char *, dev_t );

/* Exit VFS system */
extern void vfs_exit();

/* Regiser a file System to VFS */
extern int register_vfs(char *, struct vfs *(*entry)(), struct vfsops *);

/* Unregiser a file System to VFS */
extern int unregister_vfs(int );

/* open a file */
extern int vfs_open(tid_t , char *, int *, mode_t );

/* close a file */
extern int vfs_close(tid_t , int );

/* Read from a file */
extern ssize_t vfs_read(tid_t , int , void *, size_t );

/* Write to a file */
extern ssize_t vfs_write(tid_t , int , void *, size_t);

extern struct vnode *get_root_vnode();

extern struct vnode *vfs_namev(tid_t , char *, int *, mode_t );

extern int vgetattr(struct vnode *, struct vattr *);

extern int vputattr(struct vnode *, struct vattr *);

extern int vfs_access(tid_t , char *, int );

extern int vfs_mount(tid_t , char *, char *,
	int , char *, void *, size_t );

extern int vfs_umount(tid_t , char *);

extern int vfs_chmod(tid_t , char *, mode_t );

extern int vfs_stat(tid_t , char *, struct vattr *);

extern int vfs_fstat(tid_t , int , struct vattr *);

extern int vfs_fchmod(tid_t tid, int fdesc, mode_t mode);

extern int vfs_chown(tid_t , char *, uid_t , gid_t );

extern int mount_root(const char *, dev_t );

extern char *fs_getcwd(tid_t , char *);

extern int fs_chdir(tid_t , char *);

extern int do_exec(tid_t , char *, struct vn_object *);

extern int data_request(tid_t , struct vn_object *, void *);


#endif /* __VFS_H */
