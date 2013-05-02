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
 *
 * Machine independent interface for the machine dependent part of vm.
 */

#ifndef __KERNEL_PMAP_H
#define __KERNEL_PMAP_H

#include <kernel/types.h>
#include <kernel/vm_param.h>
#include <asm/pmap.h>

/* the pmap structure for kernel */
//extern struct pmap kernel_pmap;

/* Only called during initialization */
void pmap_init();

/* Gives the total amount of free physical memory */
u32 get_mem_free();

/* Allocate physical memory of given size */
vm_offset_t alloc_physical_mem(size_t size);

/* Map the page paddr to virtual address vaddr */
void pmap_enter(pmap_t map, vm_offset_t paddr, vm_offset_t vaddr, u32 protection);

/* Create a new PMAP and return its address */
extern pmap_t pmap_create();

/* Make a copy of the pmap src
 * It is used by fork.
 */
pmap_t pmap_copy(pmap_t src);

#endif /* __KERNEL_PMAP_H */

