/*
 * FS and I/O manager for XXX OS
 * Copyright (C) 2004, 2005	Manish Regmi (regmi dot manish at gmail dot com)
 *				Rajesh R.C (rajee5283 at hotmail dot com)
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
 * File: vfs.c
 * 		We are implementing the vnode/vfs architecture.
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <mrx/lock.h>
#include <mrx/list.h>
#include <mrx/atomic.h>
#include <asm/syscall.h>

#include "vfs.h"
#include "blkdev.h"
#include "chrdev.h"

#include "ext2fs/ext2.h"

extern struct vnodeops	blkops;
extern struct vnodeops	chrops;
extern struct linked_list mnted;

/* Hash table for the vnode */
struct vnodehash *vnhash = NULL;
struct vnode *vnodecache = NULL;
/* Lock for hash table */
pthread_spinlock_t vnhashlock = SPIN_LOCK_UNLOCKED;

/* File Systems Supported */
struct vfssw *fs = NULL;
pthread_spinlock_t fslock = SPIN_LOCK_UNLOCKED;

/* LRU list of vnode */
struct linked_list vnfreelist = {NULL, NULL};

extern struct vfs *rootvfs;

/* Lock for LRU free list */
pthread_spinlock_t vnflistlock = SPIN_LOCK_UNLOCKED;

int fs_index = 0;

/* Lock and enqueue */
#define VNLIST_ENQUEUE(vn)		\
{								\
	pthread_spin_lock(&vnflistlock);	\
	list_enqueue(&vnfreelist, vn, v_freelist, struct vnode *);	\
	pthread_spin_unlock(&vnflistlock);	\
}

/* Lock and dequeue */
#define VNLIST_DEQUEUE(vn)		\
{								\
	pthread_spin_lock(&vnflistlock);	\
	list_dequeue(&vnfreelist, vn, v_freelist, struct vnode *);	\
	pthread_spin_unlock(&vnflistlock);	\
}

struct vfsinit init[] = {
	{ext2fs_init, ext2fs_exit},
	{NULL, NULL}
};

/*
 * Insert the vnode to the global hash table.
 * The hash table must be locked.
 */
static inline void vnhash_insert(struct vnode  *vnode)
{
	struct vnodehash *bucket;

	pthread_spin_lock(&vnhashlock);
	bucket = &vnhash[hash_fn(vnode->v_inum, vnode->v_rdev)];
	vnode->v_next = bucket->vn;
	bucket->vn = vnode;
	pthread_spin_unlock(&vnhashlock);
}

/*
 * Remove the Vnode from the global hash table.
 * The hash table must be locked.
 */
static inline void vnhash_remove(struct vnode *vnode)
{
	struct vnodehash *bucket;
	struct vnode *p, *link;

	pthread_spin_lock(&vnhashlock);
	bucket = &vnhash[hash_fn(vnode->v_inum, vnode->v_rdev)];
	p = bucket->vn;

	if((bucket == NULL) || (p == NULL))
		return ;

	if(p == vnode)
	{
		p = vnode->v_next;
	}
	else
	{
		for(link = p;(link->v_next != vnode) || (link->v_next != NULL);
		link = link->v_next)
		;
		link->v_next = vnode->v_next;
	}
	pthread_spin_unlock(&vnhashlock);
}


/*
 * Find vnode by its device number and block number
 * The hash table must be locked.
 */
struct vnode *find_vnode(dev_t dev ,ino_t inum)
{
	struct vnodehash *bucket;
	struct vnode *p,*link;

	pthread_spin_lock(&vnhashlock);
	bucket = &vnhash[hash_fn(inum, dev)];
	p = bucket->vn;

	if((bucket == NULL) || (p == NULL))
		goto err;

	if((p->v_rdev == dev) && (p->v_inum == inum))
	{
		link = p;
		goto ok;
	}
	else
	{
 		for(link=p->v_next; link != NULL; link = link->v_next)
			if((link->v_rdev == dev) && (link->v_inum == inum))
				goto ok;
	}
 err:
 	pthread_spin_unlock(&vnhashlock);
 	return NULL;

ok:
	pthread_spin_unlock(&vnhashlock);
 	return link;
}

/*
 * Get the vnode and disk inode for root
 */
int mount_root(const char *name, dev_t dev)
{
	struct vfs *vfs;
	struct vfssw *vsw;
	int index = 0, err;

	pthread_spin_lock(&fslock);
	do{
		vsw = &fs[index];
//		printf("init found %x %s %s ", vsw->vsw_init, vsw->vsw_name, name);
		if(vsw->vsw_name == NULL)
			goto nofs;
		if(strcmp(vsw->vsw_name, name) == 0)
			break;
		index++;
	}while(vsw->vsw_init != NULL);
	pthread_spin_unlock(&fslock);
	
//	printf("VSW found; ");
	if(vsw->vsw_init == NULL)
		goto nofs;
	vfs = vsw->vsw_init();
//	printf("VSW init called; ");
	rootvfs = vfs;

	vfs->vfs_dev = dev;

	if(!vfs->ops || !vfs->ops->vfs_mount)
		return -1;

	err = vfs->ops->vfs_mount(vfs, 0);
//	printf("vfs_mount called; ");
	if(err < 0)
		return err;


	list_add(&mnted, vfs, vfs_next, struct vfs *);
//	printf("List Added; ");

	return 0;
nofs:
	printf("Fs type %s not supported.\n", name);
	return -1;
}


/* Get the vnode of root(/) */
struct vnode *get_root_vnode()
{
	return rootvfs->vfs_mpoint;
}

/* Allocate a vnode but first search in the cache. */
struct vnode *vget(struct vnode *vnode, ino_t inum, dev_t dev)
{
	struct vnode *vn;

	if(!vnode->ops || !vnode->ops->vop_read_inode)
		return NULL;

	vn = find_vnode(dev, inum);
//	printf("errg vn %d %d", vn, vn->v_inum);
	if(vn != NULL)
	{
		pthread_mutex_lock(&vn->smlock);
		atomic_inc(&vn->v_count);
		goto ok;
	}
	pthread_spin_lock(&vnflistlock);

	//if(!vnode)
	//	return NULL;

busywait:
	/* If Not in hash queue and list is empty */
	if(list_isempty(&vnfreelist))
		goto busywait;

	list_dequeue(&vnfreelist, vn, v_freelist, struct vnode *);
	pthread_mutex_lock(&vn->smlock);
	pthread_spin_unlock(&vnflistlock);

	if(vn->v_flag & V_FREE)
	{
		vn->v_count = 1;
		vn->vfsp = vnode->vfsp;
	}
	else
	{
		vnhash_remove(vn);
		atomic_inc(&vn->v_count);
	}

	vn->v_rdev = dev;
	vn->v_inum = inum;
//	printf("reading inode %d ", inum);
	vnode->ops->vop_read_inode(vn, inum);
	if(S_ISCHR(vn->v_type))
	{
		vn->ops = &chrops;
//		printf(" chrops %X vnops %X chropen %X vnopen %X", &chrops, vn->ops, chrops.vop_open, vn->ops->vop_open);
	}
	else if(S_ISBLK(vn->v_type))
		vn->ops = &blkops;

	vnhash_insert(vn);

ok:
	pthread_mutex_unlock(&vn->smlock);
	return vn;
}

/* Release a Vnode */
void vput(struct vnode *vn)
{
	pthread_mutex_lock(&vn->smlock);
	atomic_dec(&vn->v_count);

	if(vn->v_count != 0)
		goto ok;

	if(vn->v_flag & V_DIRTY)
		vn->ops->vop_write_inode(vn);

	vn->v_flag = V_FREE;
	VNLIST_ENQUEUE(vn);
ok:
	pthread_mutex_unlock(&vn->smlock);
}


/* initialize VFS */
void vfs_init(const char *name, dev_t dev)
{
	struct vnode *vnode;
	int i = 0, ret;
	init_t ini;
	void *v;

	printf("\nInitializing VFS.");
	/* Requested FS will be allocated here */

	fs = malloc(MAX_DEF_FS * sizeof(struct vfssw));
	memset((void *)fs, 0, MAX_DEF_FS * sizeof(struct vfssw));
	fs_index = 0;

	/* Call the initialization functions for each fs */
 	do{
		ini = init[i].init;
		if(!ini)
			continue;
		ini();
		i++;
	}while(ini);

//	printf(" fs init done");
	vnhash = (struct vnodehash *)malloc(VNODE_HASH_SZ);
	memset(vnhash, 0, VNODE_HASH_SZ);
	
	/*
	 * Create a Vnode Cache for 2000 entries
	 * Shouldn't the size be dynamic?
	 */
	vnodecache = (struct vnode *)malloc(2000 * sizeof(struct vnode));
	memset(vnodecache, 0, 2000 * sizeof(struct vnode));
//	printf("Vnode Cache Done.");
	list_init(&vnfreelist);
	list_init(&mnted);
	
//	v = malloc(4096);
	
//	printf("LL3 %x ll %x ", v, vnodecache);
	for(i = 0; i < 2000 ;i++)
	{
		vnode = &vnodecache[i];
		vnode->v_flag = V_NEW|V_FREE;
		list_enqueue(&vnfreelist, vnode, v_freelist, struct vnode *);
	}

	/* Initialize the Buffer Cache */
	bcache_init();
//	printf("Buffer Cache Done\n");
	/* Now mount the root File System */
	ret = mount_root(name, dev);
	if(ret < 0)
	{
		printf("Unable to mount root %s %x .\n", name, dev);
	}

	printf("\nDev %X fs = %s mounted as root. %d \n", dev, name, 2000 * sizeof(struct vnode));
}

/* Exit VFS system */
void vfs_exit()
{
	free(vnodecache);
	free(fs);
}

/* Regiser a file System to VFS */
int register_vfs(char *name, struct vfs *(*entry)(), struct vfsops *ops)
{
	pthread_spin_lock(&fslock);
	fs[fs_index].vsw_init = entry;
	fs[fs_index].vswops = ops;
	fs[fs_index].vsw_name = name;
//	printf("Registered %s at %x ", fs[fs_index].vsw_name, fs[fs_index].vsw_init);
	pthread_spin_unlock(&fslock);

	fs_index++;
	return fs_index-1;
}

/* Unregiser a file System to VFS */
int unregister_vfs(int i)
{
	pthread_spin_lock(&fslock);
	fs[i].vsw_init = NULL;
	fs[i].vswops = NULL;
	pthread_spin_unlock(&fslock);

	return i;
}
