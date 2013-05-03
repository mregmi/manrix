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
 * File: vm.c
 *
 * This is the architecture independent part of the VM system.
 *
 **/

#include <kernel/vm.h>
#include <kernel/vm_page.h>
#include <kernel/vm_slab.h>
#include <kernel/vm_map.h>
#include <kernel/kernel_stdio.h>
#include <kernel/pmap.h>
#include <kernel/error.h>
#include <kernel/thread.h>

/* The vm_map for the kernel. */
extern struct vm_map kernel_vmmap;

/* The kernel object. Composes of all kernel memory. */
extern struct vm_object kernel_object;

extern vm_offset_t start_va;

void vm_init()
{
	list_init(&kernel_object.page);

	/* Initialize the vm_page */
	vm_page_init();

	/* Initialize the slab allocator */
	slab_init();

	vm_map_init();

	vm_object_init();

	/* Reinitialize the pmap */
	pmap_init();

}

/* Initilaize the kernel memory. */
/*void kmem_init()
{
	error_t err;
	vm_offset_t va = start_va;

	kprintf("Inside kmem init\n");
	err = vm_map_enter(&kernel_vmmap, &va, KERNEL_END-KERNEL_BASE, &kernel_object, 0);
	if(err)
		panic("Kmem init failed");
}
*/
/*
 * Allocate zero filled pages in the map
 */
int vm_allocate(vm_map_t map, vm_offset_t *address, vm_size_t size)
{
	*address = map->start;
	align_page(size);

	vm_map_enter(map, address, size, NULL, VM_MAP_ENTRY_ZERO_FILL);
//	kprintf("Memory at %X ", *address);
	return 0;
}

/*
 * Deallocate the virtual mem from the map.
 */
int vm_deallocate(vm_map_t map, vm_offset_t offset, vm_size_t size)
{
	page_base(offset);
	align_page(size);

	return vm_map_remove(map, offset, size);
}

/*
 * vm_map: map the file or Anonymous memory to the map.
 */
void *vm_map(vm_map_t map, struct vn_object *vno, int type)
{
	int ret, prot = PAGE_USER;
	vm_object_t object;
	vm_offset_t pa = vno->va;
	
//	kprintf("\t\t\t\t\t\tvmmap dd %d %x", current->tid, pa);
	object = vm_object_create(vno->size);
	if(!object)
		return (void *)(-ENOMEM);
	
	if(vno->protection & VM_MAP_ENTRY_WRITE)
		prot |= PAGE_RW;
	
	/* take this as a zero fill page */
	if(!vno->va && !(type & VM_MAP_ENTRY_ZERO_FILL))
	{
//		kprintf("dddooonnneee");
		type |= VM_MAP_ENTRY_ZERO_FILL;
	}
	
	if(!(type & VM_MAP_ENTRY_OBJECT_FIXED))
		vno->va = map->start;

	object->vn = *vno;
	page_base(vno->va);
	ret = vm_map_enter(map, &vno->va, vno->size, object, type);
		
	if((type & VM_MAP_ENTRY_ANON) && !(type & VM_MAP_ENTRY_ZERO_FILL))
	{
		
		if(vno->map_type & VM_MAP_ENTRY_WRITE)
			prot = PAGE_RW;
//		kprintf("ANON MAP ENTERED va = %X pa %x size = %x off %d prot %x map %x.", vno->va, pa, \
			vno->size,vno->offset, vno->protection, type);
		pmap_enter_range(map->pmap, pa, vno->va, vno->size, prot|PAGE_USER);

	}

	return (void *)vno->va;
}


