/**
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
 **/

/**
 * File: vm_slab.c
 *
 * Machine independent interface for the machine dependent part of vm.
 * We use slab allocator for our kernel memory allocation.
 * It is very much similar as in paper presented in usenix.
 * We use bufctl array to keep track of free objects.
 **/

#include <string.h>

#include <kernel/vm_slab.h>
#include <kernel/kernel_stdio.h>
#include <kernel/vm_param.h>
#include <kernel/list.h>
#include <kernel/vm_page.h>
#include <kernel/pmap.h>


extern struct vm_map kernel_vmmap;
extern vm_offset_t start_va;
extern struct pmap kernel_map;

/* The kernel object. Composes of all kernel memory. */
extern struct vm_object kernel_object;


/* A constructor for cache_cache */
static void cache_const(void *object, kmem_cache_t cache)
{
	kmem_cache_t obj = (kmem_cache_t) object;
	list_init(&obj->full);
	list_init(&obj->partial);
	list_init(&obj->free);
	obj->object = 0;
	obj->waste = 0;
	obj->nobjects = 0;
	obj->align = 0;
//	obj->name = NULL;
	obj->next_cache = NULL;
}

static struct kmem_cache cache_cache = {
				{NULL,NULL},
				{NULL,NULL},
				{NULL,NULL},
				sizeof(struct kmem_cache),
				0, 0, 0,
				cache_const,
				NULL,
				"cache_cache",
				NULL
				};

/* keep track of list of caches. */
kmem_cache_t cachep = &cache_cache;

/* Calculate the no of objects and wasted memory */
static inline void cache_calculate(kmem_cache_t cache, size_t *nobj, size_t *waste)
{
	size_t free;
	free = PAGE_SIZE - sizeof(struct kmem_slab);
	*nobj = free / (cache->object + sizeof(bufctl_t));
	*waste = free % (cache->object + (sizeof(bufctl_t)));
}

/* Fill the slab by calling the constructor */
static inline void slab_fill(kmem_slab_t slab, kmem_cache_t cache)
{
	void *obj;
	int i;

	slab->objaddr = (((char *)slab) + sizeof(struct kmem_slab) +(cache->nobjects * sizeof(bufctl_t)));
	slab->free = 0;
	slab->nentries = 0;

//	kprintf("\nSlab_fill %x at %x %s", (vm_offset_t)slab, slab->objaddr, cache->name);
	for(i = 0 ; i < cache->nobjects; i++)
	{
		bufctl_slab(slab)[i] = i+1;
		obj = slab->objaddr + (i * cache->object);

		if(cache->ctor)
			cache->ctor(obj, cache);

		if(cache->dtor)
			cache->dtor(obj, cache);
	}
	bufctl_slab(slab)[i-1] = BUFCTL_END;
}

/* Initialize the slab allocator */
void slab_init()
{

	list_init(&cache_cache.full);
	list_init(&cache_cache.partial);
	list_init(&cache_cache.free);
	//cache_cache.ctor = cache_const;
	SPIN_LOCK_INIT(&cache_cache.lock);
	cache_calculate(&cache_cache,&cache_cache.nobjects, &cache_cache.waste);
}

/* Grow the slab in cache by one */
static inline void kmem_cache_grow(kmem_cache_t cache)
{
	kmem_slab_t slab;

	slab = (kmem_slab_t)kmem_alloc(PAGE_SIZE);

//	kprintf("slab %x",slab);

	slab_fill(slab,cache);
//	cache->nextpartial = (list_t)slab;
//	slab->list.prev = &cache->partial;
	list_add(&cache->partial, slab, list, kmem_slab_t);
}


/* Create a new cache */
kmem_cache_t kmem_cache_create(const char *name, size_t size, u8 align, \
			void (*ctor)(void *, kmem_cache_t), void (*dtor)(void *, kmem_cache_t))
{
	kmem_cache_t cache;

	/* some checks */
	if((!name) || (!size))
		return NULL;

	cache = kmem_cache_alloc(&cache_cache, 0);

	if(!cache)
	{
		kprintf("Cannot create cachedd\n");
		return NULL;
	}

//	memset(cache, 0, sizeof(struct kmem_cache));

	cache->ctor = ctor;
	cache->dtor = dtor;
	cache->object = size;
	list_init(&cache->full);
	list_init(&cache->partial);
	list_init(&cache->free);
	SPIN_LOCK_INIT(&cache->lock);
	strcpy(cache->name, name);

//	kprintf("Cache at Address = %X\n",cache);

	cache_calculate(cache, &cache->nobjects, &cache->waste);

	if(!cache->nobjects)
	{
		kprintf("Cannot create cache\n");
		kmem_cache_free(&cache_cache, cache);
		return NULL;
	}

	/* update the cache's next pointer. */
	cachep->next_cache = cache;
	cachep = cache;
	cache->next_cache = NULL;

	return cache;
}

/* Allocate an object in the given cache */
void *kmem_cache_alloc(kmem_cache_t cache, u32 flags)
{
	void *objp;
	kmem_slab_t slab;
	kmem_slab_t temp;
	
	spin_lock(&cache->lock);
	/*
	 * Check if there are any slab in the partial list. If not we need to
	 * allocate in free slab. And if it is not available there we need to
	 * grow the slab by one.
	 */
	if(cache->nextpartial == &cache->partial)
	{
		if(cache->nextfree == &cache->free)
		{
//			kprintf("\nnoc..");
			kmem_cache_grow(cache);
		}
		else
		{
//			kprintf(" yoc ");
			temp = cache->nextfree;
			list_remove(&cache->free, temp, list, kmem_slab_t);
			list_add(&cache->partial, temp, list, kmem_slab_t);
		}
	}

	slab = (kmem_slab_t)cache->nextpartial;

	objp = slab->objaddr + (slab->free * cache->object);
	slab->free = bufctl_slab(slab)[slab->free];
	slab->nentries += 1;
	spin_unlock(&cache->lock);
//	kprintf("kmem_cache_alloc:- slab=%X,objp=%x.objaddr=%x, %d\n",slab,objp,slab->objaddr, slab->nentries);
	return objp;
}


/* FIXME: CHECK IN THE FULL LIST */
int kmem_cache_free(kmem_cache_t cache, void *obj)
{
	kmem_slab_t slab;
	bufctl_t nobj;
	kmem_slab_t new;

//	kprintf("Kmem_cache_free:- %x. \t\t", (vm_offset_t)obj);
	///////////////////////////////////////// problem .1
	spin_lock(&cache->lock);
	slab = (kmem_slab_t)cache->nextpartial;

	if(!slab->nentries)
		goto free;

	nobj = (obj - slab->objaddr)/cache->object;
	bufctl_slab(slab)[nobj] = slab->free;
	slab->free = nobj;

	slab->nentries -= 1;

	if(slab->nentries <= 0)
	{
free:
		new = cache->nextpartial;
		list_remove(&cache->partial, new, list, kmem_slab_t);
		list_add(&cache->full, new, list, kmem_slab_t);
	}
	
	spin_unlock(&cache->lock);
	return 0;
}

static inline void free_all(kmem_cache_t cache)
{
	kmem_slab_t tmp;
	
	spin_lock(&cache->lock);
	while(cache->nextfree != &cache->free)
	{
		tmp = cache->nextfree;
		list_remove(&cache->free, tmp, list, kmem_slab_t);
		page_dealloc(&kernel_object, (vm_offset_t)tmp);
	}
	spin_unlock(&cache->lock);	
}

void kmem_cache_reap()
{
	kmem_cache_t next = &cache_cache;
	
	spin_lock(&cache_cache.lock);
	while(next->next_cache != NULL)
	{
		free_all(next);
		next = next->next_cache;
	}
	spin_unlock(&cache_cache.lock);
}


/* dummy */
void dm()
{
	kmem_cache_t ch = &cache_cache;
	while(ch->next_cache)
	{
		kprintf("- %s %x -", ch->name, ch->nextpartial);
		ch = ch->next_cache;
	}
}


