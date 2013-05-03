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
 * 		Architecture independent VM declarations and Macros.
 */

#ifndef __VM_VM_PARAM_H
#define __VM_VM_PARAM_H

#include <asm/vm_param.h>
#include <kernel/types.h>

/* Kernel virtual memory base */
#define KERNEL_BASE			0xE0000000
#define KERNEL_END			0xFFFFFFFF

#define VM_PAGE_FREE			0x01
#define VM_PAGE_DIRTY			0x02
#define VM_PAGE_ACTIVE			0x04
#define VM_PAGE_SWAPABLE		0x08

#define VM_COPY_MEMORY			0x01
#define VM_REF_MEMORY			0x02


/* Linear address to virtual */
#define linear_to_virtual(addr)			((vm_offset_t) (addr) + KERNEL_BASE)
/* Virtual address to linear address */
#define virtual_to_linear(addr)			((vm_offset_t) (addr) - KERNEL_BASE)


#endif /* __VM_VM_PARAM_H */
