/*
 * FS and I/O manager for ManRiX OS
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
 *
 */
 /*pthread_
 * File: blkdev.h
 * 			Block device Declarations.
 */

#ifndef __BLKDEV_H
#define __BLKDEV_H

#include <sys/types.h>
#include <inttypes.h>
#include <pthread.h>

#include <mrx/list.h>
#include <mrx/blkdev.h>

#include "vfs.h"

/* Operation types */
#define BLK_READ		1
#define BLK_WRITE		2

#define blk_read(device, block, size, data)	\
		blk_strategy(BLK_READ, device, block, size, data)

#define blk_write(device, block, size, data)	\
		blk_strategy(BLK_WRITE, device, block, size, data)

/*
 * The Partition Info used by disk drives.
 * The info is independent of partition scheme.
 */
 struct partition_info{
 	unsigned long start;		/* starting block. (multiple of sector_size) */
 	unsigned long size;		/* size of the partition.  (multiple of sector_size) */
 	uint16_t ptype;			/* Partition Type */
 	uint16_t flag;
  };

/*
 * Structure defining a Block device.
 * For Block Devices that have different structures for different disk,
 * we implement an n array of block_device.
 * For eg, Each IDE disk may have different partitions, sectorsize etc.
 * further each disk is capable of working independently.
 */
struct block_device{
	struct partition_info	*pinfo;
	int npartitions;
	dev_t dev;
	int sectsize;
	spinlock_t listlock;	/* Lock the list */
};

/* The operations Permitted in Block devices. */
struct block_operations{
	int (* d_open)(struct operation *);
	int (* d_close)(struct operation *);
	int (* d_strategy)(struct operation *);
	int (* d_size)();
};

/* Block device Switch */
struct bdevsw{
	struct block_operations *ops;
	struct block_device *device;
	tid_t server_id;
	int ndevs;
};

/* Register the Block device */
#ifdef SEPERATE_IO_PROCESS
 extern int register_blockdev(int major, tid_t server_id, int ndevs);
#else
 extern int register_blockdev(int , struct block_operations *, int);
#endif

extern int unregister_blockdev(int );
extern int blockdev_open(struct vnode *);
extern int blockdev_close(struct vnode *);
extern int blockdev_read(struct file *, size_t , void *);
extern int blockdev_write(struct file *file, size_t size, void *data);
extern int blk_strategy(int type, dev_t device, size_t block, size_t blksize, void *data);

extern int blk_init();
extern int blk_exit();

extern int io_main();

#endif	/* __BLKDEV_H  */
