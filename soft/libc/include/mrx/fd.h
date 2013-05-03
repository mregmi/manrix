/*
 * Libc for ManRiX OS
 * Copyright (C) 2004, 2005
 * 	Manish Regmi (regmi dot manish at gmail dot com) and
 * 	Rajesh R.C   (rajesh dot rc at gmail dot com)
 *
 * This Program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This Program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
/*
 * File: fd.h
 * 		The file descriptor implementation.
 */

#ifndef __LIBC_FD_H
#define __LIBC_FD_H

#include <sys/types.h>

#define FDESC_CREATE(svr, fd)	((svr << 16) | fd)
#define FDESC_FD(fd)			(fd & 0xFFFF)
#define FDESC_SVR(fd)			((fd >> 16) & 0xFFFF)

/*
 * The max (major) number of Block devices
 * Should't this be Dynamic?????
 */
#define MAX_MAJOR		32

#define MAJOR(dev)		(dev >> 16)
#define MINOR(dev)		(dev & 0xFFFF)

#define DEV_CHAR		1
#define DEV_BLK			2

struct indev{
	ino_t inode;
	dev_t dev;
};

/* The open file object */
struct file {
	mode_t  f_access;	/* opened mode */
	off_t	f_offset;	/* offset in the file. */
	int 	f_ref;		/* reference count */

	struct indev vnode;
};

/* uid, gid. */
struct userid{
	int uid, euid;
	int gid, egid;
};

#define put_openfile syscall_fd_alloc

extern int fd_alloc(tid_t );
extern int fd_free(tid_t , int);
extern int get_openfile(tid_t , int , struct file *);

#endif /* __LIBC_FD_H */
