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
 * File: vm_map.h
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

#ifndef __KERNEL_VM_MAP_H
#define __KERNEL_VM_MAP_H

#include <kernel/config.h>
#include <kernel/pmap.h>
#include <kernel/types.h>
#include <kernel/vm_param.h>
#include <kernel/list.h>
#include <kernel/error.h>
#include <kernel/spinlock.h>
#include <kernel/atomic.h>

#define VM_MAP_ENTRY_ZERO_FILL		1		/* Entry is zero fill */
#define VM_MAP_ENTRY_OBJECT_ANYWHERE	2		/* The object in the entry can be mapped anywhere. */
#define VM_MAP_ENTRY_OBJECT_FIXED	4		/* The address should be fixed */
#define VM_MAP_ENTRY_ANON		8		/* The object in the map has anonymous Mapping */
#define VM_MAP_ENTRY_LOWMEM		0x40		/* Lower memory (< 16MB) */
#define VM_MAP_ENTRY_FILE		0x80		/* Memory Mapped File */

/* attr of vm_map_entry */
#define VM_MAP_ENTRY_READ		1
#define VM_MAP_ENTRY_WRITE		2
#define VM_MAP_ENTRY_EXECUTE		4
#define VM_MAP_ENTRY_NONE		8
#define VM_MAP_ENTRY_LOCKED		16

/* attr of vm_map */
#define VM_LOCK				3
#define VM_MCL_CURRENT			1
#define VM_MCL_FUTURE			2

/*
 * A vm_map structure to hold a mapping
 */
struct vm_map_entry {
	struct linked_list entries;

#define  next_entry entries.next
#define  prev_entry entries.prev

	vm_offset_t		start;
	vm_offset_t		end;
	struct vm_object	*object;
	u16			type;
	u16 		attr;
	spinlock_t lock;
};

typedef struct vm_map_entry *vm_map_entry_t;

struct vm_map {
	struct vm_map_entry *vmentry;
	struct pmap	*pmap;
	vm_size_t	size;
	atomic_t	ref_count;	/* References to this vm_map */
	vm_offset_t	start;
	vm_offset_t	end;
	vm_map_entry_t	hint;	/* Hint pointing to the entry that resolved the pagefault. */
	vm_offset_t private;	/* Arch's can have their own arch specfic value */
	int attr;
	spinlock_t vmlock;
};


typedef struct vm_map *vm_map_t;

/* Initialize the vm_map */
extern void vm_map_init();

/* create a new vm_map structure */
extern vm_map_t vm_map_create();

/* create a new entry in the given map.
 *Create an object if null object is passed.
 */
extern int vm_map_enter(vm_map_t , vm_offset_t *, vm_size_t , struct vm_object *, int );

/* Duplicate the given map according to the given flag.
 * 1. Duplicate the whole map including pages. Used by fork.
 * 2. Just make a reference. Used by vfork.
 */
extern vm_map_t vm_map_fork(vm_map_t , u32 );

/* make a copy of all the vm_entrys. */
extern void vm_entry_copy(vm_map_t , vm_map_t );

/* Find the entry by the address. */
extern vm_map_entry_t vm_map_lookup_entry(vm_map_t , vm_offset_t );

extern void vm_map_destroy(vm_map_t );

/* Get the free virtual memory in the given map */
extern vm_offset_t get_vaddr(vm_map_t , vm_size_t );

/* Free the virtual address from the given map of size "size" */
extern int free_vaddr(vm_map_t , vm_offset_t , vm_size_t );

extern int handle_cow(vm_map_t , vm_map_entry_t , vm_offset_t );

extern int data_request(vm_map_entry_t , vm_offset_t );

extern int vm_map_remove(vm_map_t , vm_offset_t , size_t );

extern int vm_lock_all(vm_map_t );

extern int vm_unlock_all(vm_map_t );

extern int vm_lock(vm_map_t , vm_offset_t , vm_size_t , int );

extern int vm_unlock(vm_map_t , vm_offset_t , vm_size_t , int );

extern int vm_protect(vm_map_t , vm_offset_t , vm_size_t , int );

extern int vm_share_addr(vm_map_t , vm_map_t , vm_offset_t *, size_t , int );

extern int vm_unshare_addr(vm_map_t , vm_offset_t , size_t );

extern void exit_vm_map(vm_map_t );

#endif /* __KERNEL_VM_MAP_H */
