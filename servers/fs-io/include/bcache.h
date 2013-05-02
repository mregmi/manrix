/*
 * FS and I/O manager for XXX OS
 * Copyright (C) 2004, 2005	Manish Regmi (manish underscore regmi at hotmail dot com)
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
 *
 * File: bcache.h
 */

#ifndef __BCACHE_H
#define __BCACHE_H

#include <sys/types.h>
#include <stdint.h>
#include <mrx/lock.h>

#include <mrx/list.h>


#define B_DIRTY  1
#define B_FREE   2
#define B_BUSY   4
#define B_LOCK   8
#define B_VDATA	 16

#define MINSZ	 10  /* used to get the index of freelist array */

#define FREELIST 5   /*for each block size ie 512,1024,2048,4096,8192 */

#define HSIZE_TABLE 4096


struct bcache{
	int b_status;                   /*status of buffer cache */
	struct bcache *next;          /* pointer for hash queue */
	struct linked_list b_freelist;  /* pointer to freelist buffer cache */
	dev_t b_devno;                  /* device number */
	uint32_t b_blockno;             /* block number */
	int b_ioerror;			        /* i/o error  */
	void *b_data;			        /* pointer to the data */
	size_t b_datasz;		    	/* size of the data */

	pthread_mutex_t smlock;
};

struct bhash{
	struct bcache *bcache;
};

typedef struct bhash bhash_t;

#define hash_fn(blockno,devno) (((unsigned)(blockno^devno))%(HSIZE_TABLE/sizeof(bhash_t)))

extern void bcache_init();

extern int bwrite(dev_t , uint32_t , void *, size_t );

extern int brelse (struct bcache *);

extern struct bcache *bread(dev_t , uint32_t , unsigned int );

extern struct bcache *getblk(dev_t , uint32_t , size_t );

#endif	/* __BCACHE_H */
