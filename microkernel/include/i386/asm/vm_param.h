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
 * File:- vm_param.h
 *
 * X86 specific Page table attributes.
 *  and Architecture Specfic Macros.
 */

#ifndef __I386_VM_PARAM_H
#define __I386_VM_PARAM_H

#include <kernel/types.h>

/* We use 4k pages only */
#define PAGE_SIZE		0x1000
#define PDE_SHIFT		22
#define PDE_MASK		0x3FF
#define PTE_SHIFT		12
#define PTE_MASK		0x3FF


#define PAGE_PRESENT		0x001		/* present Bit */
#define PAGE_RW			0x002		/* Read/write bit */
#define PAGE_USER		0x004		/* User/Superuser */
#define PAGE_WRTH		0x008		/* Write through */
#define PAGE_CACHE_DISABLE	0x010		/* Page Cache Disable */
#define PAGE_ACCESSED		0x020		/* Accessed Bit */
#define PAGE_DIRTY		0x040		/* Dirty Bit */
#define PAGE_SZ			0x080		/* Page size in pde, reserved for pte */
#define GLOBAL			0x100		/* Global bit */
//#define AVAIL			0xE00


/* Number of page table entries in a frame */
#define NTABLES			0x400	/* 1024 entries */

/* Page tables for kernel starts from here */
#define KTABLE_START	(NTABLES-128)

#define MAX_INT_VAL		0xFFFFFFFF
#define BITS_PER_INT		32

#define USER_STACK_TOP		0xDFEFFFFF
#define USER_STACK_ENTRY_SIZE		0x200000
/* Memory offset */
typedef unsigned int	vm_offset_t;

typedef unsigned int	vm_size_t;
/* page directory entry */
typedef unsigned int	pde_t;

/* Convert address to page directory entry number */
#define addr_to_pdenum(addr)			((((vm_offset_t)addr) >> PDE_SHIFT) & PDE_MASK)
/* Convert address to page table entry number */
#define addr_to_ptenum(addr)			((((vm_offset_t)addr) >> PTE_SHIFT) & PTE_MASK)

#define addr_to_pte(addr)				(((vm_offset_t)addr) & 0xfffff000)

#define addr_to_page(addr)				(((vm_offset_t)addr) >> PTE_SHIFT)


/* Align the address to page boundry */
#define	align_page(addr)		((addr) = (((vm_offset_t) (addr)) + (PAGE_SIZE-1)) & ~(PAGE_SIZE-1))
#define page_base(addr)			((addr) = (((vm_offset_t)(addr)) & ~(PAGE_SIZE-1)))
#define get_aligned(addr)		((((vm_offset_t) (addr)) + (PAGE_SIZE-1)) & ~(PAGE_SIZE-1))
#define get_base(addr)			((((vm_offset_t) (addr)) & ~(PAGE_SIZE-1)))
#endif	/* __I386_VM_PARAM_H */
