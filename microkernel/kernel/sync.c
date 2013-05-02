/*
 * ManRiX Microkernel
 * Copyright (C) 2004, 2005
 * 	Manish Regmi (regmi dot manish at gmail dot com) and
 * 	Rajesh R.C   (rajee5283@hotmail.com)
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

/*	file: sync.c
 * 		.
 */

#include <string.h>

#include <kernel/sync.h>
#include <kernel/scheduling.h>
#include <kernel/vm.h>

/* Hash table of all the sync structures. */
synchash_t shash = { NULL };

/* Sync cache */
kmem_cache_t sync_cache;

/* Insert the sync object in the global hash table. */
static inline void shash_insert(struct sync *s)
{
	synchash_t p;

	p = &shash[sync_fn(s->owner, s->sync_id)];
	s->next = p->sync;
	p->sync = s;
}

/* Remove the sync object from the global hash table. */
static inline void shash_remove(struct sync *s)
{
	synchash_t bucket;
	struct sync *p, *link;

	bucket = &shash[sync_fn(s->owner, s->sync_id)];
	p = bucket->sync;

	if((p->owner == s->owner) && (p->sync_id == s->sync_id))
	{
		p = s->next;
	}
	else
	{
		for(link = p; (link->next != NULL); link = link->next)
			if((link->next->owner == s->owner) && (link->next->sync_id == s->sync_id))
				link->next = s->next;
	}
}

/* Find a process by its pid. */
struct sync *find_sync(pid_t pid, int sync_id)
{
	synchash_t bucket;
	struct sync *p, *link;

	bucket = &shash[sync_fn(pid, sync_id)];
	p = bucket->sync;

	/* optimize for average case. */
	if((p->owner == pid) && (p->sync_id == sync_id))
		return p;
	else
	{

		for(link=p->next; (link->owner != pid)&&(link->sync_id != sync_id); link=link->next)
			if(link->next == NULL)
				return NULL;
	}
	return link;
}

/* Initialize the synchronization */
void sync_init()
{
	shash = (synchash_t)kmem_alloc(4096);
	memset(shash, 0, 4096);

	sync_cache = kmem_cache_create("Sync cache", sizeof(struct sync), 32, NULL, NULL);
}

/* Create a sync structure */
int syscall_sync_create(void *ptr, int type)
{
	struct sync *sn;
	process_t p = current->proc;

	sn = kmem_cache_alloc(sync_cache, 0);
	if(!sn)
		return -ENOMEM;
	sn->sync_type = type;
	sn->owner = p->pid;
	sn->sync_id = p->sync.id++;
	list_init(&sn->wq);
	sn->next = NULL;
	shash_insert(sn);
	list_add(&p->sync.syncs, sn, syncs, struct sync *);

	return sn->sync_id;
}


/* Destroy a sync structure */
int syscall_sync_destroy(void *ptr, int sync_id)
{
	struct sync *sn;
	process_t p = current->proc;

	sn = find_sync(current->proc->pid, sync_id);
	if(!sn)
		return -EINVAL;

	list_del(&p->sync.syncs, sn, syncs, struct sync *);
	shash_remove(sn);
	kmem_cache_free(sync_cache, sn);

	return 0;
}

/* Lock the sync object */
int syscall_sync_lock(void *ptr, int sync_id)
{
	struct sync *sn;
	thread_t thr = current;
//	kprintf("SYNC locked");
	sn = find_sync(thr->proc->pid, sync_id);
	if(!sn)
		return -EINVAL;

	thr->states = SYNC_BLOCKED;
	remove_thread(thr);
	list_enqueue(&sn->wq, thr, runq, thread_t);
	schedule();

	return 0;
}

/* Unlock the sync object */
int syscall_sync_unlock(void *ptr, int sync_id)
{
	struct sync *sn;
	thread_t thr;

	sn = find_sync(current->proc->pid, sync_id);
	if(!sn)
		return -EINVAL;

	list_dequeue(&sn->wq, thr, runq, thread_t);
	thr->states = THREAD_RUNNING;
	enqueue_thread(thr);

	return 0;
}

/* Delete all sync belonging to the given process */
void delete_sync(struct syncstruct *snc)
{
	struct sync *s;

	while(1)
	{
		if(list_isempty(&snc->syncs))
			return;

		list_dequeue(&snc->syncs, s, syncs, struct sync *);
		if(!s)
			return;
		shash_remove(s);

		kmem_cache_free(sync_cache, s);
	}
}
