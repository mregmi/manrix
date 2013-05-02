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
 * File: vm_object.h
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

#ifndef __KERNEL_VM_OBJECT_H
#define __KERNEL_VM_OBJECT_H

#include <kernel/vm_map.h>
#include <kernel/list.h>
#include <kernel/types.h>
#include <kernel/vm_param.h>
#include <kernel/spinlock.h>

/* Object attributes */
#define COW_IN_PROGRESS		1
#define COPY_ON_WRITE		2
#define OBJECT_SHADOW		4


/* Object For vnode */
struct vn_object{
	int fd;
	int blk_size;
	size_t page_size;
	vm_offset_t va;
	unsigned long offset;
	int map_type;
	int protection;
	size_t size;
	int pager_id;	/* The pager can be default and user defined as well. */
	/* memory address which does not require disk access */
	vm_offset_t noload_start, noload_end;
};

struct vm_object {
	struct linked_list	page; /* List of pages belonging to this object. */
	atomic_t		ref_count;
	atomic_t		npages;
	vm_size_t		size; /* size of the object. */
	struct vm_object *shadow;	/* shadow to this object */
	int			attribute;
	struct vn_object vn;
	
	spinlock_t lock;
};

typedef struct vm_object *vm_object_t;



/* Initialize the vm_object */
extern void vm_object_init();

/* Create a new object of size size */
extern vm_object_t vm_object_create(size_t size);

/* copy an object. COW the pages. */
extern vm_object_t cow_pages(struct vm_map *map, vm_object_t old, vm_offset_t start, vm_offset_t end);

extern void delete_object(vm_object_t );

#endif /* __KERNEL_VM_OBJECT_H */
