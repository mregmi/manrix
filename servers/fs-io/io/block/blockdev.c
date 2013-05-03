/*
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
 */
/*
 * File: blockdev.c
 *
 * The generic interface for Block devices.
 */

#include "svrconfig.h"
 
#include <stdlib.h>
#include <string.h>
#include <mrx/blkdev.h>
#include <mrx/fscases.h>

#include "blkdev.h"
#include "bcache.h"
#include "vfs.h"

//#include "partition.h"


struct bdevsw	bdevs[MAX_MAJOR];

struct vnodeops	blkops = {
			.vop_open = blockdev_open,
			.vop_close = blockdev_close,
			.vop_strategy = blk_strategy,
			//.vop_write = blockdev_write,
			};

/* Register The Block Device Driver */
#ifdef SEPERATE_IO_PROCESS
 int register_blockdev(int major, tid_t server_id, int ndevs)
#else
 int register_blockdev(int major, struct block_operations *ops, int ndevs)
#endif /* SEPERATE_IO_PROCESS */
{
	struct block_device *device;
	int index;

	if((major >= MAX_MAJOR) || (major < 0))
		return -1;

	device = (struct block_device *) malloc(sizeof(struct block_device) *
				ndevs);
	memset(device, 0, sizeof(struct block_device) *	ndevs);
	for(index = 0; index < ndevs; index++)
	{
		device[index].dev = DEVICE(major, index, 0);
		device[index].sectsize = 512;
	}
#ifdef SEPERATE_IO_PROCESS
	bdevs[major].server_id = server_id;
#else
	bdevs[major].ops = ops;
	bdevs[major].device = device;
#endif
	bdevs[major].ndevs = ndevs;

	return 0;
}

#ifndef SEPERATE_IO_PROCESS
/* Register The Block Device Driver */
int unregister_blockdev(int major)
{
	if((major >= MAX_MAJOR) || (major < 0))
		return -1;
	bdevs[major].ops = NULL;
	free(bdevs[major].device);
	bdevs[major].device = NULL;

	return 0;
}
#endif

int blk_init()
{
	memset(&bdevs, 0, sizeof(struct bdevsw) * MAX_MAJOR);
	
//	printf(" blk init");
#ifndef SEPERATE_IO_PROCESS
//#include "storage/ata/ata.h"
//	fdc_init();
	ata_init();
#endif
	return 0;
}

int blk_exit()
{
	return 0;
}

/* Open the Block device */
int blockdev_open(struct vnode *vn)
{
	struct bdevsw *bdev;
	struct block_device *dev;
	int major, disk;

	major = MAJOR(vn->v_rdev);
	disk = DISKNO(vn->v_rdev);

	bdev = &bdevs[major];

#ifdef SEPERATE_IO_PROCESS
	return bdev->server_id;
#else
	dev = &bdev->device[disk];
	
	return bdev->ops->d_open(NULL);
#endif /* SEPERATE_IO_PROCESS */
}

/* Close the block device */
int blockdev_close(struct vnode *vn)
{
	struct bdevsw *bdev;
	struct block_device *dev;
	int major, disk;

	major = MAJOR(vn->v_rdev);
	disk = DISKNO(vn->v_rdev);

	bdev = &bdevs[major];
	dev = &bdev->device[disk];

#ifndef SEPERATE_IO_PROCESS
	return bdev->ops->d_close(NULL);
#endif

}

/* Read from the Block device */
int blockdev_read(struct file *file, size_t size, void *data)
{

	return 0;
}

/* Write to the Block device */
int blockdev_write(struct file *file, size_t size, void *data)
{
	return 0;
}


/* Block device strategy */
int blk_strategy(int type, dev_t device, size_t block, size_t blksize, void *data)
{
	struct bdevsw *bdev;
	int major;
	struct operation blkop;


	major = MAJOR(device);

	bdev = &bdevs[major];
	
	blkop.type = type;
	blkop.device = device;
	blkop.buffer = data;
	blkop.blksize = blksize;

	blkop.block = block;
//	printf("Initial svr %d ", bdev->server_id);
//	for(;;);
#ifdef SEPERATE_IO_PROCESS
	while(bdev->server_id == 0)
		;//sched_yield();	
	printf("DONE for %d \n", bdev->server_id);
	return notify_blk_server(bdev->server_id, &blkop);
#else
	return bdev->ops->d_strategy(&blkop);
#endif

}

#ifdef SEPERATE_IO_PROCESS
#include <asm/ipc.h>
int notify_blk_server(tid_t server, struct operation *op)
{
	struct message msg;

	msg.flags = MSG_LONG_SEND|MSG_MAP_RECEIVE;
	msg.w1 = SVR_STRATEGY;
	msg.snd_ptr = op;
	msg.snd_bufsize = sizeof(struct operation);
	msg.rcv_ptr = op->buffer;
	msg.rcv_bufsize = op->blksize;

	return send_message(server, &msg);
}
#endif
