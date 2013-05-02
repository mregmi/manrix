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
 * File: blkdev.h
 * 			Block device Declarations.
 */

#ifndef __FSIO_BLKDEV_H
#define __FSIO_BLKDEV_H

#include <sys/types.h>

#define BLKDEV_READ	1
#define BLKDEV_WRITE	2
#define BLKDEV_STRATEGY 3


/* The disk no is the upper 8 bits of the Minor number */
#define DISKNO(dev)		((MINOR(dev) >> 8) & 0xff)

/* The partition number is the lower 8 bits of the Minor Number */
#define PARTNO(dev)		(MINOR(dev) & 0xFF)

#define DEVICE(major, disk, part)	((major << 16)|(disk << 8)| part)

/* A Block device read/write operation. */
struct operation{
	dev_t device;
	pthread_t caller;
	int		type;		/* Operation type */
	unsigned long	block;		/* Lba value to read/write */
	size_t 		blksize;
	void		*buffer;	/* Buffer to store read/write data */
};

#endif	/* __FSIO_BLKDEV_H  */

