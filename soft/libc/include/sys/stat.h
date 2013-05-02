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
 * File: sys/stat.h
 * 		File statistics.
 */

#ifndef __SYS_STAT_H
#define __SYS_STAT_H

#ifndef __SYS_TYPES_H
#include <sys/types.h>
#endif	/* __SYS_TYPES_H */

#ifndef __TIME_H
#include <time.h>
#endif	/* __TIME_H */

/* File Format */
#define S_IFMT			0xF000
#define S_IFSOCK		0xC000
#define S_IFLNK			0xA000
#define S_IFREG			0x8000
#define S_IFBLK			0x6000
#define S_IFDIR			0x4000
#define S_IFCHR			0x2000
#define S_IFIFO			0x1000

/* access right */
#define S_ISUID			0x0800
#define S_ISGID			0x0400
#define S_ISVTX			0x0200


#define S_IRWXU			0x01C0
#define S_IRUSR			0x0100
#define S_IWUSR			0x0080
#define S_IXUSR			0x0040

#define S_IRWXG			0x0038
#define S_IRGRP			0x0020
#define S_IWGRP			0x0010
#define S_IXGRP			0x0008


#define S_IRWXO			0x0007
#define S_IROTH			0x0004
#define S_IWOTH			0x0002
#define S_IXOTH			0x0001

/* Identifies the type of file by using i_mode of inode */
/* structure as input.									*/
#define S_ISDIR(mode)	((mode & S_IFMT) == S_IFDIR)
#define S_ISLNK(mode)	((mode & S_IFMT) == S_IFLNK)
#define S_ISBLK(mode)	((mode & S_IFMT) == S_IFBLK)
#define S_ISSOCK(mode)	((mode & S_IFMT) == S_IFSOCK)
#define S_ISREG(mode)	((mode & S_IFMT) == S_IFREG)
#define S_ISCHR(mode)	((mode & S_IFMT) == S_IFCHR)
#define S_ISFIFO(mode)	((mode & S_IFMT) == S_IFIFO)

/*
 * File Attributes
 */
struct stat{
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

int    chmod(const char *, mode_t);
int    fchmod(int, mode_t);
int    fstat(int, struct stat *);
int    lstat(const char *__restrict, struct stat *__restrict);
int    mkdir(const char *, mode_t);
int    mkfifo(const char *, mode_t);
int    mknod(const char *, mode_t, dev_t);

int    stat(const char *__restrict, struct stat *__restrict);
mode_t umask(mode_t);

#endif /* __SYS_STAT_H */
