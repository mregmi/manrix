/**
 * ManRiX Microkernel
 * Copyright (C) 2004, 2005
 * 	Manish Regmi (regmi dot manish at gmail dot com) and
 * 	Rajesh R.C   (rajesh dot rc at gmail.com)
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
/**
 * File: vm_slab.h
 *
 * Machine independent interface for the machine dependent part of vm.
 * We use slab allocator for our kernel memory allocation.
 * We use bufctl array to keep track of free objects.
 */

#ifndef __KERNEL_VM_SLAB_H
#define __KERNEL_VM_SLAB_H

#include <kernel/config.h>
#include <kernel/list.h>
#include <kernel/types.h>
#include <kernel/spinlock.h>

typedef unsigned short bufctl_t;

#define CACHE_NAME_LEN		11
#define BUFCTL_END		0xFFFF


/*
 * Structure For Slab.
 */
struct kmem_slab {
	struct linked_list		list;
	void 		*objaddr; /* Pointer to the first object */
	s16		nentries;	/* no of used objects */
	bufctl_t	free; 		/* Pointer to the next free object in the slab */
};


typedef struct kmem_slab *kmem_slab_t;

/* Structure for cache */
struct kmem_cache {

	struct linked_list	partial;
	struct linked_list	full;
	struct linked_list	free;

#define nextfull	full.next
#define prevfull	full.prev
#define nextpartial	partial.next
#define prevpartial	partial.prev
#define nextfree	free.next
#define prevfree	free.prev

	size_t			object;
	size_t			waste;		/* No of bytes wasted */
	u32			nobjects;
	u8			align;			/* Not implemented yet. */
	void	  	(*ctor)(void *, struct kmem_cache *);
	void	  	(*dtor)(void *, struct kmem_cache *);
	char		 	name[CACHE_NAME_LEN];
	struct kmem_cache	*next_cache;
	
	spinlock_t lock;
};

typedef struct kmem_cache *kmem_cache_t;

#define bufctl_slab(slab) \
	((bufctl_t *)(((struct kmem_slab *)(slab))+1))

/* Initialize the slab allocator. */
void slab_init();

/* Create a new cache */
kmem_cache_t kmem_cache_create(const char *, size_t, u8,void (*ctor)(void *, kmem_cache_t ),\
							void (*dtor)(void *, kmem_cache_t));


/* Allocate an object in the given cache */
void *kmem_cache_alloc(kmem_cache_t , u32 flags);

/* FIXME: CHECK IN THE FULL LIST */
int kmem_cache_free(kmem_cache_t cache, void *obj);

/*called by pager to extract free pages from free slab list */
void kmem_cache_reap();

#endif  /* __KERNEL_VM_SLAB_H */
