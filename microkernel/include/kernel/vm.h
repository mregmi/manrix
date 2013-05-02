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
 * File:- vm.h
 * 		This is the architecture independent part of the VM system.
 * The VM initialization routines.
 *
 **/

#ifndef __KERNEL_VM_H
#define __KERNEL_VM_H

#include <kernel/vm_object.h>
#include <kernel/vm_slab.h>
#include <kernel/vm_param.h>
#include <kernel/vm_page.h>
#include <kernel/vm_map.h>

#define kvm_allocate(vaddr, size)	vm_allocate(&kernel_vmmap, &vaddr, size)

/* Initialize the VM */
extern void vm_init();

/* Initialize the kernel memory. */
extern void kmem_init();
extern void *vm_map(vm_map_t , struct vn_object *, int );
extern int vm_deallocate(vm_map_t , vm_offset_t , vm_size_t );
extern int vm_allocate(vm_map_t , vm_offset_t *, vm_size_t );

#endif /* __KERNEL_VM_H */
