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
/*	file: fcntl.h
 * 		.
 */


#ifndef __FCNTL_H
#define __FCNTL_H

#ifndef __SYS_TYPES_H
 #include <sys/types.h>
#endif

/* Values for cmd used by fcntl() (the following values are unique) */
#define F_DUPFD			/* Duplicate file descriptor. */
#define F_GETFD			/* Get file descriptor flags. */
#define F_SETFD			/* Set file descriptor flags. */
#define F_GETFL			/* Get file status flags and file access modes. */
#define F_SETFL			/* Set file status flags. */
#define F_GETLK			/* Get record locking information. */
#define F_SETLK			/* Set record locking information. */
#define F_SETLKW		/* Set record locking information; wait if blocked. */
#define F_GETOWN		/* Get process or process group ID to receive SIGURG signals.*/
#define F_SETOWN		/* Set process or process group ID to receive SIGURG signals */

#define FD_CLOEXEC 		/* Close the file descriptor upon execution of an exec family function. */

/*
 * Values for l_type used for record locking with fcntl()
 */

#define F_RDLCK			/* Shared or read lock.*/
#define F_UNLCK			/* Unlock. */
#define F_WRLCK			/* Exclusive or write lock */

/*
 * The following values are file creation flags and are used in the oflag value to open().
 * They shall be bitwise-distinct.
 */

#define O_CREAT		0x01	/* Create file if it does not exist. */
#define O_EXCL		0x02	/* Exclusive use flag. */
#define O_NOCTTY	0x04	/* Do not assign controlling terminal. */
#define O_TRUNC		0x08	/* Truncate flag. */
#define O_APPEND 	0x10	/* Set append mode. */
#define O_DSYNC 	0x20	/* [SIO]  Write according to synchronized I/O data integrity completion. */
#define O_NONBLOCK 	0x40	/* Non-blocking mode. */
#define O_RSYNC 	0x80	/* [SIO]  Synchronized read I/O operations.  */
#define O_SYNC 		0x100	/* Write according to synchronized I/O file integrity completion. */


/* Mask for use with file access modes is as follows: */
#define O_ACCMODE	0x200	/* Mask for file access modes. */


/* File access modes used for open() and fcntl() are as follows: */
#define O_RDONLY	0x400	/* Open for reading only. */
#define O_RDWR		0xC00	/* Open for reading and writing. */
#define O_WRONLY	0x800	/* Open for writing only. */

#define O_DIRECTORY	0x1000	/* Open the Directory */
#define O_NDELAY	0x2000

/* The structure flock describes a file lock. */
struct flock{
	short  l_type;   /* Type of lock; F_RDLCK, F_WRLCK, F_UNLCK.  */
	short  l_whence; /* Flag for starting offset. */
	off_t  l_start;  /* Relative offset in bytes. */
	off_t  l_len;    /* Size; if 0 then until EOF. */
	pid_t  l_pid;    /* Process ID of the process holding the lock; returned with F_GETLK. */
};

int  creat(const char *, mode_t);
int  fcntl(int, int, ...);
int  open(const char *, int, ...);
/* [ADV] */
int  posix_fadvise(int, off_t, off_t, int);
int  posix_fallocate(int, off_t, off_t);


#endif  /* __FCNTL_H  */

