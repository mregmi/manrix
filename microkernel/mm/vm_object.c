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
 * File: vm_object.c
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

#include <kernel/vm_object.h>
#include <kernel/vm_slab.h>
#include <kernel/vm_map.h>
#include <kernel/vm_page.h>

/* cache for vm_object. */
kmem_cache_t vm_object_cache = NULL;

/* The kernel object. Composes of all kernel memory. */
struct vm_object kernel_object = {
				{NULL,NULL},
				{0},
				{0},
				KERNEL_END-KERNEL_BASE
				};


/* Initialize the vm_object. */
void vm_object_init()
{
	vm_object_cache = kmem_cache_create("vm_object_cache", sizeof(struct vm_object), 4, NULL, NULL);
}

/* create a new vm_object. */
vm_object_t vm_object_create(vm_size_t size)
{
	vm_object_t new_vm_object;

	new_vm_object = (vm_object_t) kmem_cache_alloc(vm_object_cache, 0);
	new_vm_object->size = size;
	atomic_set(&new_vm_object->ref_count, 1);
	list_init(&new_vm_object->page);
	SPIN_LOCK_INIT(&new_vm_object->lock);
	new_vm_object->shadow = NULL;

	return new_vm_object;
}

/* Delete the object */
void delete_object(vm_object_t object)
{
	if(!atomic_dec(&object->ref_count))
		return;

	free_pages(object);
	kmem_cache_free(vm_object_cache, object);
}



