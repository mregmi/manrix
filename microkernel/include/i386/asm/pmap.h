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
/*
 * File: pmap.h
 *
 * This is the architecture dependent part of the VM system.
 *
 */
/*
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
 */

#ifndef __ASM_VM_PMAP_H
#define __ASM_VM_PMAP_H

#include <kernel/vm_param.h>
#include <kernel/list.h>
#include <kernel/types.h>
#include <kernel/vm_slab.h>
#include <kernel/spinlock.h>
#include <asm/mm.h>

/* a list to identify the pages used by page tables. */
/*struct pte_list{
	struct linked_list ptenext;
	vm_page_t	page;
	u16 		nentries;
};

typedef struct pte_list *pte_list_t;
*/
/* The architecture dependent pmap structure */
struct pmap{
	struct linked_list plist;	/* A linked list of used pages for page table entries. */
	pde_t *pdbr;		/* Base address of page directory */
	spinlock_t	lock;
};

typedef struct pmap *pmap_t;

/* Map the page paddr to virtual address vaddr */
extern void pmap_enter(pmap_t , vm_offset_t , vm_offset_t , u32 );

/* Change the protection bits of the given pmap entries */
extern int pmap_protect(pmap_t , vm_offset_t , vm_offset_t , int );

extern void pmap_cow(pmap_t , vm_offset_t , vm_offset_t );

extern void pmap_share_kernel(pmap_t );

extern void pmap_remove(pmap_t , vm_offset_t , vm_offset_t );

/* Initialize pmap before slab */
extern void pmap_pre_init();
extern int pmap_destroy(pmap_t );

extern void pmap_const(void *, kmem_cache_t );

extern vm_offset_t alloc_physical_mem(u32 );

extern void pmap_enter_range(pmap_t , vm_offset_t , vm_offset_t , vm_size_t , int );
extern u32 get_total_mem();

extern void exec_pmap(pmap_t );


#endif /* __ASM_VM_PMAP_H */
