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
 * File: vm_page.h
 *
 * This is the architecture independent part of the VM system.
 *
 **/

/**
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University.
 * Copyright (c) 1993,1994 The University of Utah and
 * the Computer Systems Laboratory (CSL).
 * All rights reserved.
 *
 * Permission to use, copy, modify and distribute this software and
 * its documentation is hereby granted, provided that both the
 * copyright notice and this permission notice appear in all copies of
 * the software, derivative works or modified versions, and any
 * portions thereof, and that both notices appear in supporting
 * documentation.
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

#ifndef __KERNEL_VM_PAGE_H
#define __KERNEL_VM_PAGE_H

#include <kernel/list.h>
#include <kernel/vm_param.h>
#include <kernel/types.h>
#include <kernel/vm_map.h>
#include <kernel/atomic.h>
#include <kernel/spinlock.h>


/* page flag values */
#define VM_PAGE_READ		1
#define VM_PAGE_WRITE		2
#define VM_PAGE_EXEC		4
#define VM_PAGE_WIRED		16

/* Page allocation type */
#define PAGE_ALLOC_DMA		2
#define PAGE_ALLOC_KER		1
#define PAGE_ALLOC_OTH		0

/* structure to keep track of each physical frame */
struct vm_page {
  /* List of the pages to which they belong */
	struct linked_list	pagelist;

#define  pnext	 pagelist.next
#define  pprev	 pagelist.prev

	struct vm_page	*next;	/* hash table next link. */
	vm_offset_t		base;	/* physical base address */
	struct vm_object	*object; /* object i belong to. */
	vm_offset_t		offset;	/* Offset in that object. */
	atomic_t		count;	/* No of references to this page */
	u16			flags;
	
	spinlock_t lock;
};

typedef struct vm_page *vm_page_t;

struct page_hash{
	vm_page_t page;
};

typedef struct page_hash *page_hash_t;

extern void vm_page_init();

/* deallocate a page from given map with given virtual address */
extern void page_free(vm_page_t );

/* return a page from free page list. */
extern vm_page_t get_page(int );

/* Allocate "size of kernel memory" */
extern vm_offset_t kmem_alloc(vm_size_t );

extern int kmem_free(vm_offset_t, vm_size_t);
/* Insert a page in the object, offset. */
extern void vm_page_insert(vm_page_t , struct vm_object *, vm_offset_t );

/* Remove the page from the object offset */
extern void vm_page_remove(vm_page_t );

/* Find a page in the object with the given offset */
extern vm_page_t vm_page_lookup(struct vm_object *, vm_offset_t );

extern void free_pages(struct vm_object *);

/* allocate a page from free list to the object. */
extern vm_page_t page_alloc(struct vm_object *, vm_offset_t );

extern int create_vm_page(vm_offset_t , vm_offset_t , vm_map_entry_t );

extern int page_dealloc(struct vm_object *, vm_offset_t );

extern void vm_lock_object(vm_map_entry_t );

extern void vm_unlock_object(vm_map_entry_t );

extern int alloc_zero_fill_page(vm_map_t , vm_map_entry_t , vm_offset_t );

#endif  /* __KERNEL_VM_PAGE_H */
