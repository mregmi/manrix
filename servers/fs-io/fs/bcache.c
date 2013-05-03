/*
 * FS  for XXX OS
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
 * File: bcache.c
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/sysinfo.h>

#include <mrx/list.h>

#include "bcache.h"
#include "vfs.h"
#include "blkdev.h"




/* maximum size of buffercache */
size_t maxbuffcache = 0;

/* current size of buffer cache */
size_t curbuffcache = 0;

/* Pointer to the bcache hash table. */
bhash_t *bhash_table = NULL;

/* freelist for each block size ie 512 ,1024 etc*/
struct linked_list freelist[FREELIST];

/*
 * Insert the bcache to the global hash table.
 * The hash table must be locked.
 */
static inline void bhash_insert(struct bcache  *bcache)
{
	bhash_t *bucket;

	bucket = &bhash_table[hash_fn(bcache->b_blockno, bcache->b_devno)];
	bcache->next =bucket->bcache;
	bucket->bcache= bcache->next;

}

/*
 * Remove the bcache from the global hash table.
 * The hash table must be locked.
 */
static inline void bhash_remove(struct bcache  *bcache)
{
	bhash_t *bucket;
	struct bcache *p, *link;

	bucket = &bhash_table[hash_fn(bcache->b_blockno,bcache->b_devno)];
	p = bucket->bcache;

	if(p == bcache)
	{
		p = bcache->next;
	}
	else
	{
		for(link = p;(link->next != NULL);link = link->next)
			if(link == bcache)
				link->next = bcache->next;
	}


}

/*
 * Find bcache by its device number and block number
 * The hash table must be locked.
 */
 struct bcache *find_bcache(int dev ,int block)
 {
 	bhash_t *bucket;
 	struct bcache *p,*link;

 	bucket = &bhash_table[hash_fn(block, dev)];
  	p = bucket->bcache;

	if(p == NULL)
		return NULL;

  	if((p->b_devno == dev) && (p->b_blockno == block))
  		return p;
  	else
 	{

 		for(link=p->next; link != NULL; link=link->next)
 			if(link->b_devno == dev && link->b_blockno == block)
				return link;
 	}


 	return NULL;
}

static inline int list_index(int size)
{
	int bit = 0;

	while(size >= 1)
	{
//		printf("o ");
		size >>= 1;
		bit++;
	}
	return (bit - MINSZ);
}

/* Allocate buffer cache */
struct bcache *alloc_bufcache(dev_t dev, uint32_t block, size_t size)
{
	struct bcache *bcache;
//	struct linked_list *list;

	bcache = (struct bcache *) malloc(sizeof(struct bcache));
	if(!bcache)
		return NULL;
	memset(bcache, 0, sizeof(struct bcache));
	//	printf("bread okd %x", bcache);
	bcache->b_status = B_FREE;
//	bcache->next = NULL;
	bcache->b_devno = dev;
	bcache->b_blockno = block;
	bcache->b_datasz = size;
	bcache->b_ioerror = 0;
	bcache->b_data = malloc(size);
//	list_init(&bcache->b_freelist);
//      printf("bread %x ", bcache->b_data);
	if(!bcache->b_data)
	{
		free(bcache);
		return NULL;
	}
//
	if(blk_read(dev, block, size, bcache->b_data) < 0)
		return NULL;
//		printf("bread okxxx");
//	bcache->b_datasz = size;
//	list = &freelist[list_index(bcache->b_datasz)];
//	list_enqueue(list, bcache, b_freelist, struct bcache *);
	bhash_insert(bcache);
	curbuffcache += size;
//		printf("bread okyyy");
	return bcache;
}

/* get the block using device number and block number */
struct bcache *getblk(dev_t dev,uint32_t block,size_t size)
{
	struct bcache *bcache;
	struct linked_list *list;
	bcache = find_bcache(dev, block);
	if(bcache)
		return bcache;

	if(curbuffcache >= maxbuffcache)
	{

		list = &freelist[list_index(size)];
		list_dequeue(list, bcache, b_freelist, struct bcache *);

//		bhash_remove(bcache);
		bcache->b_devno = dev;
		bcache->b_blockno = block;

		blk_read(dev, block, size, bcache->b_data);

//		list_enqueue(list, bcache, b_freelist, struct bcache *);
		bhash_insert(bcache);

		return bcache;
	}
//	printf("oow");
	bcache = alloc_bufcache(dev,block,size);
	return bcache;

 }

/* Read Block */
struct bcache *bread(dev_t dev, uint32_t block, unsigned int bsize)
{
	struct bcache *bcache;
//	struct linked_list *list;

	bcache = getblk(dev, block, bsize);
	if(bcache == NULL)
	 	return NULL;

//	pthread_mutex_lock(&bcache->smlock);
	bcache->b_status = B_BUSY;

/*	list = &freelist[list_index(bsize)];

back:
	if(list_isempty(list))
		goto back;

	list_dequeue(list, bcache, b_freelist, struct bcache *);
*/
//	pthread_mutex_unlock(&bcache->smlock);
	return bcache;
}

int brelse (struct bcache *bcache)
{
	struct linked_list *list;
	int index;
//	struct bcache *bcace;

//	bcace = find_bcache(bcache->b_devno, bcache->b_blockno);
	index = list_index(bcache->b_datasz);
//	printf("index %d", index);
	list = &freelist[index];
	//list_dequeue(list,bcace,b_freelist,struct bcache *);

	bcache->b_status = B_FREE;
	//phash_remove(bcace):
	list_enqueue(list, bcache, b_freelist, struct bcache *);
	//phash_insert(bcache);
	return 0;
}

/* Write block */
int bwrite(dev_t dev, uint32_t block, void *data, size_t bsize)
{
	struct bcache *bcache;

	bcache = getblk(dev, block, bsize);
	if(bcache == NULL)
		return -1;
	pthread_mutex_lock(&bcache->smlock);
	blk_write(dev, block, bsize, data);
	bcache->b_status = B_DIRTY;
	pthread_mutex_unlock(&bcache->smlock);
	return 0;
}



/* Initilizing bcache */
void bcache_init()
{
	struct sysinfo sinfo;
	size_t freemem;
	int i;

	syscall_sysinfo(&sinfo);

	freemem = (sinfo.total_mem - sinfo.used_mem);
	maxbuffcache = (freemem/100) * 50;
	curbuffcache = 0;
	printf("\nBuffer Cache: Dedicating %d KB of Free Memory for Buffer Cache.\n", freemem/1024);

	bhash_table = (bhash_t *)malloc(HSIZE_TABLE);
	memset(bhash_table, 0, HSIZE_TABLE);

	for(i = 0 ;i < FREELIST; i++)
		list_init(&freelist[i]);

}
