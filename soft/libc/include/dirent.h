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
 *	dirent.h - format of directory entries
 */

#ifndef __DIRENT_H
#define __DIRENT_H

#include <sys/types.h>
#include <stdint.h>

#define NAME_MAX	256
#define _DIRENT_HAVE_D_RECLEN	1

struct __dirstream
{
    int fd;			/* File descriptor.  */

    char *data;			/* Directory block.  */
    size_t allocation;		/* Space allocated for the block.  */
    size_t size;		/* Total valid data in the block.  */
    size_t offset;		/* Current offset into the block.  */

    off_t filepos;		/* Position of next entry to read.  */

};

typedef struct __dirstream DIR;

#define _DIR_dirfd(dirp)	((dirp)->fd)

struct dirent{
	ino_t  d_ino;      /* File serial number. */
	uint16_t d_reclen;
	uint8_t d_namelen;
	uint8_t d_filetype;
	char   d_name[NAME_MAX];   /* Name of entry. */
};

int            closedir(DIR *);
DIR           *opendir(const char *);
struct dirent *readdir(DIR *);
int            readdir_r(DIR *__restrict, struct dirent *__restrict,
                   struct dirent **__restrict);
void           rewinddir(DIR *);
void           seekdir(DIR *, long);
long           telldir(DIR *);


#endif /* __DIRENT_H */
