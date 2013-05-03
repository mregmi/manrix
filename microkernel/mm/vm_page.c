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
 * File: vm_page.c
 * This is the architecture independent part of the VM system.
 * The handling of pages are done here.
 **/

/**
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University.
 * Copyright (c) 1993,1994 The University of Utah and
 * the Computer Systems Laboratory (CSL).
 * All rights reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON, THE UNIVERSITY OF UTAH AND CSL ALLOW FREE USE OF
 * THIS SOFTWARE IN ITS "AS IS" CONDITION, AND DISCLAIM ANY LIABILITY
 * OF ANY KIND FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF
 * THIS SOFTWARE.
 *
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 **/

#include <string.h>

#include <kernel/vm_page.h>
#include <kernel/list.h>
#include <kernel/pmap.h>
#include <kernel/kernel_stdio.h>
#include <kernel/vm_object.h>
#include <kernel/info.h>
#include <kernel/vm.h>

extern struct pmap kernel_map;
extern struct vm_map kernel_vmmap;
extern struct vm_object kernel_object;
extern struct sysinfo sysinfo;
/*
 * Keep track of free pages.
 * It is the head for the linked list of free pages.
 * We are using three lists.
 * 1) for memory less than 16MB. This is usually used for DMA transfer on PC.
 * 2) For memory greater than 16MB but less than 512MB. This is the max address
 *    addressable from kernel.
 * 3) For rest of the memory
 */
//struct linked_list dma_page_list = {NULL, NULL};	/* For DMA */
//struct linked_list ker_page_list = {NULL, NULL};	/* For Kernel */
struct linked_list free_page_list[3] = {{NULL, NULL}, {NULL, NULL}, {NULL, NULL}};	/* other */
spinlock_t freelock = SPIN_LOCK_UNLOCKED;

/* Total no of free pages */
atomic_t nfree_pages = {0};

/* Hash table for pages. */
page_hash_t pagehash = NULL;
spinlock_t pagehashlock = SPIN_LOCK_UNLOCKED;

/* mask for the hash function. */
unsigned int pagehash_mask = 0;

static vm_size_t nfree, ntotal;
static vm_page_t page_page;
static int vm_page_initialized = 0;

#define phash(x) ((((x)>>8)^(x))&(1024 - 1))

/* Hash function */
#define page_hash(object, offset) phash(((vm_offset_t)(object)) + (vm_offset_t)addr_to_page(offset))

//((((vm_offset_t)(object)) + (vm_offset_t)addr_to_page(offset)) & pagehash_mask)

/* Put the page to the free list */
static inline void put_to_free_list(vm_page_t page)
{
	list_t list;

	spin_lock(&freelock);
/*	if(page->base < 0x1000000)	/* less than 16MB */
/*	{
		list = &free_page_list[PAGE_ALLOC_DMA];
//		kprintf("DMA: %X ", page->base);
	}
	else if(page->base < 0x20000000)	/* less than 512 MB */
	{
		list = &free_page_list[PAGE_ALLOC_KER];
//		kprintf("KER: %X ", page->base);
	}
/*	else
	{
		list = &free_page_list[PAGE_ALLOC_OTH];
		kprintf("OTH: %X ", page->base);
	}
*/
	list_enqueue(list, page, pagelist, vm_page_t);
	spin_unlock(&freelock);
}

/* get the page from free list */
static inline vm_page_t get_from_free_list(int flag)
{
	vm_page_t page;
	list_t list;

	spin_lock(&freelock);
/*
	switch(flag)
	{
		case PAGE_ALLOC_DMA:
			list = &free_page_list[PAGE_ALLOC_DMA];
			kprintf("From DMA: ");
			break;
		case PAGE_ALLOC_KER:*/
			list = &free_page_list[PAGE_ALLOC_KER];
			/*kprintf("From KER: ");
			break;
		case PAGE_ALLOC_OTH:
			list = &free_page_list[PAGE_ALLOC_OTH];
			kprintf("From OTH: ");
			break;
		default:
			spin_unlock(&freelock);
			return NULL;
	}
	/* If the Page is not available in the required list,
	 * look on other list. Cannot look in Lower valued list.
	 */
/*	while(list_isempty(list))
	{
		kprintf("OTH Empty ");
		if(++flag > PAGE_ALLOC_DMA)
		{
			spin_unlock(&freelock);
			return NULL;
		}
		list = &free_page_list[flag];
	}
*/
	list_dequeue(list, page, pagelist, vm_page_t);
	spin_unlock(&freelock);
//	kprintf("%X ", page->base);
	return page;
}

/* We track each frame by vm_page. */
void vm_page_init()
{
	vm_size_t npages, size; /* no of pages reqd for page_page */
	vm_page_t page;
	//unsigned int nbuckets;
	int i;
//	page_hash_t new;

	nfree = (get_mem_free()/PAGE_SIZE);
	ntotal = (get_total_mem()/PAGE_SIZE);
	size = (ntotal * sizeof(struct vm_page));
	align_page(size);
	npages = (size / PAGE_SIZE) + 1;

	page_page = (vm_page_t) alloc_physical_mem(npages);

	/* Now, we create a hash table of all of the pages. */
//	while(nbuckets < ntotal)
//		nbuckets <<= 1;
//	pagehash_mask = nbuckets - 1;

	for(i = 0 ; i < 3; i++)
		list_init(&free_page_list[i]);

	/* Now, Put all the pages in the free page list. */
	for(i = 0; i < nfree ; i++)
	{
		page = &page_page[i];
		atomic_init(&page->count);
		page->base = get_next_frame();
		if(page->base == 0)
			break;
 		put_to_free_list(page);
		atomic_inc(&nfree_pages);
		page->flags = VM_PAGE_FREE;
	}

	pagehash = (page_hash_t)alloc_physical_mem(1);
	memset(pagehash, 0, PAGE_SIZE);

	vm_page_initialized = 1;
}

/* Insert a page in the object, offset. */
void vm_page_insert(vm_page_t page, vm_object_t object, vm_offset_t offset)
{
	page_hash_t bucket;

	page->object = object;
	page->offset = offset;
	spin_lock(&pagehashlock);
	bucket = &pagehash[page_hash(object, offset)];
	page->next = bucket->page;
	bucket->page = page;
	spin_unlock(&pagehashlock);
//	list_add(&object->page, page, pagelist, vm_page_t);
}


/* Remove the page from the object offset */
void vm_page_remove(vm_page_t page)
{
	page_hash_t bucket;
	vm_page_t   new;

	spin_lock(&pagehashlock);
	bucket = &pagehash[page_hash(page->object, page->offset)];

	/* remove the page from the hash. */
	if(bucket->page == page)
		bucket->page = page->next;
	else
	{
	/*	for(new = bucket->page; (new->next != page)||(new->next != NULL) ; new = new->next)
			;//fix if not found
		new->next = page->next;*/
		for(new=bucket->page; new != NULL; new=new->next)
		{
			if(new == page)
			{
				new->next = page->next;
				break;
			}
		}
	}
	spin_unlock(&pagehashlock);
	/* Also remove the page from the object's pagelist. */
//list_del(&page->object->page, page, pagelist, vm_page_t);
}

/*
 * Create a new page.
 *  It is used where some pages cannot be initialised by
 * vm_page_init like user modules.
 */
int create_vm_page(vm_offset_t pa, vm_offset_t offset, vm_map_entry_t entry)
{
	vm_page_t page;

	if(!entry->object)
		entry->object = vm_object_create(PAGE_SIZE);

	nfree++;
	if(nfree > ntotal)
	{
		kprintf("Memory not sufficient");
		return -1;
	}

	page = &page_page[nfree];
	page->base = page_base(pa);
	atomic_set(&page->count, 1);
	page->next = NULL;
//	kprintf("creating page p = %x v = %x", page->base, page_base(offset));
	vm_page_insert(page, entry->object, page_base(offset));
 	list_add(&entry->object->page, page, pagelist, vm_page_t);

	return 0;
}

/* Find a page in the object with the given offset */
vm_page_t vm_page_lookup(vm_object_t object, vm_offset_t offset)
{
	page_hash_t bucket;
	vm_page_t new;

	spin_lock(&pagehashlock);
	bucket = &pagehash[page_hash(object, offset)];

	if((bucket->page->object == object) && (bucket->page->offset == offset))
	{
		new = bucket->page;
		spin_unlock(&pagehashlock);
		return new;
	}
	for(new=bucket->page; new != NULL; new=new->next)
	{
		if((new->object == object) && (new->offset == offset))
		{
			spin_unlock(&pagehashlock);
			return new;
		}
	}
//end:
	spin_unlock(&pagehashlock);
	return NULL;
}


/* Make the resident pages of the object Locked. */
void vm_lock_object(vm_map_entry_t entry)
{
	vm_object_t object = entry->object;
	vm_page_t page;

	page = (vm_page_t)object->page.next;
	for(; page != (vm_page_t)&object->page; page = (vm_page_t)page->pnext)
	{
		if((page->offset >= entry->start) && (page->offset <= entry->end))
			page->flags |= VM_PAGE_WIRED;
	}
}


/* Unlock the resident pages of the object. */
void vm_unlock_object(vm_map_entry_t entry)
{
	vm_object_t object = entry->object;
	vm_page_t page;

	spin_lock(&object->lock);
	page = (vm_page_t)object->page.next;
	for(; page != (vm_page_t)&object->page; page = (vm_page_t)page->pnext)
	{
		if((page->offset >= entry->start) && (page->offset <= entry->end))
			page->flags &= ~VM_PAGE_WIRED;
	}
	spin_unlock(&object->lock);
}

/* Put the page to the free_list. */
static inline void put_page(vm_page_t pg)
{
	put_to_free_list(pg);
	spin_lock(&syslock);
	sysinfo.used_mem -= PAGE_SIZE;
	sysinfo.used_ram = sysinfo.used_mem;
	spin_unlock(&syslock);
}

/* Deallocate the page from the object */
int page_dealloc(vm_object_t object, vm_offset_t offset)
{
	vm_page_t page;

	page = vm_page_lookup(object, offset);
	if(!page)
		return -1;
	if(atomic_dec(&page->count))
		return 0;

	spin_lock(&page->lock);
	vm_page_remove(page);
	spin_lock(&object->lock);
	list_del(&object->page, page, pagelist, vm_page_t);
	spin_unlock(&object->lock);
	put_page(page);
	spin_unlock(&page->lock);

	return 0;
}


/*
 * Deallocate all the pages corresponding to the given entry.
 */
void remove_pages(vm_map_entry_t entry)
{
	vm_object_t object;
	vm_offset_t offset = page_base(entry->start);

	object = entry->object;
	while(offset < entry->end)
	{
		page_dealloc(object, offset);
		offset += PAGE_SIZE;
	}
}

/* Free all the pages of the given object */
void free_pages(vm_object_t object)
{
	vm_page_t page = NULL;

	spin_lock(&object->lock);
	while(!list_isempty(&object->page))
	{
		list_remove(&object->page, page, pagelist, vm_page_t);
		if(!page)
		{
			kprintf("free_pages: error removing page\n");
			return;
		}
		if(atomic_dec(&page->count))
			return;
		vm_page_remove(page);
		put_page(page);
	}
	spin_unlock(&object->lock);
}


/* allocate a page from free list to the object. */
vm_page_t page_alloc(vm_object_t object, vm_offset_t offset)
{
	vm_page_t page;

	if(!object)
		object = vm_object_create(PAGE_SIZE);

	page = (vm_page_t)get_page(PAGE_ALLOC_OTH);

	if(!page)
		kprintf("No more free pages. Wake the pagedemon later.");

	atomic_set(&page->count, 1);
	spin_lock(&object->lock);
	vm_page_insert(page, object, page_base(offset));
	list_add(&object->page, page, pagelist, vm_page_t);
	spin_unlock(&object->lock);

	spin_unlock(&page->lock);	/* get_page() returns locked page */

	return page;
}

/* Deallocate a page */
void page_free(vm_page_t page)
{
	vm_page_remove(page);
	list_del(&page->object->page, page, pagelist, vm_page_t);
	put_page(page);
}



/* deallocate a page from given map with given virtual address */
void kmem_dealloc(vm_map_t map, vm_offset_t vaddr)
{
	vm_map_entry_t ventry;
	vm_object_t object;
	vm_page_t page;

	ventry = vm_map_lookup_entry(map, vaddr);
	if(!ventry)
		return;

	object = ventry->object;
	while(object != NULL)
	{
		page = vm_page_lookup(object, vaddr);
		if(page)
		{
			page_free(page);
			break;
		}

		/* search through all the shadows. */
		object = object->shadow;
	}
}


/* return a page from free page list. */
vm_page_t get_page(int flag)
{
	vm_page_t page;

	if(!vm_page_initialized)
		panic("get_page(): get_page used before memory initialization\n");

	page = get_from_free_list(flag);
	if(!page || (page == (vm_page_t)&free_page_list[flag]))
	{
		kprintf("MANRIX_BUG when allocating page\n");
		return NULL;
	}
	SPIN_LOCK_INIT(&page->lock);
	spin_lock(&page->lock);
	atomic_dec(&nfree_pages);
	list_init(&page->pagelist);
	page->next = NULL;
	page->flags = 0;

	sysinfo.used_mem = sysinfo.used_ram = (sysinfo.total_mem - (atomic_get(&nfree_pages) * PAGE_SIZE));
	return page;
}

/* Allocate "size of kernel memory" */
vm_offset_t kmem_alloc(vm_size_t size)
{
	int npages, index;
	vm_page_t page;
	vm_offset_t start;

	npages = align_page(size)/PAGE_SIZE;

	page = get_page(PAGE_ALLOC_KER);
	spin_lock(&kernel_object.lock);
	vm_page_insert(page, &kernel_object, page->base);
	list_add(&kernel_object.page, page, pagelist, vm_page_t);

	start = page->base;
	spin_unlock(&page->lock);
	pmap_enter(&kernel_map, start, start+KERNEL_BASE, PAGE_RW);

	for(index = 1; index < npages ; index++)
	{
		page = get_page(PAGE_ALLOC_KER);
		vm_page_insert(page, &kernel_object, page->base);
		list_add(&kernel_object.page, page, pagelist, vm_page_t);
		pmap_enter(&kernel_map, page->base, page->base+KERNEL_BASE, PAGE_RW);
		spin_unlock(&page->lock);
	}

	spin_unlock(&kernel_object.lock);
	return (start);
}

int kmem_free(vm_offset_t va, vm_size_t size)
{
	kmem_dealloc(&kernel_vmmap, va);

	return 0;
}

/*
 * allocate a zero fill page.
 * used by page faults.
 */
int alloc_zero_fill_page(vm_map_t map, vm_map_entry_t entry, vm_offset_t address)
{
	vm_page_t page;
	
	if(entry->object == NULL)
		entry->object = vm_object_create(PAGE_SIZE);
	page_base(address);
	page = page_alloc(entry->object, address);
	if(!page)
		return -1;
		
	pmap_enter(map->pmap, page->base, address, PAGE_USER|PAGE_RW);
	
	return 0;
}
 
