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
 * File: vm_pager.c
 *
 * This is the architecture independent part of the VM system.
 * The paging mechanism is handled here.
 **/

#include <kernel/vm_object.h>
#include <kernel/vm_slab.h>
#include <kernel/vm_map.h>
#include <kernel/vm_page.h>
#include <kernel/message.h>
#include <kernel/scheduling.h>
#include <kernel/kernel_stdio.h>

#include <asm/asm.h>

#define FS_DATA_REQ		0

int data_request_all(vm_map_entry_t entry, vm_offset_t va)
{
	struct message msg;
	vm_map_t map = current->proc->map;
	struct vn_object *vno = &entry->object->vn;
	struct vm_page *page;
	int pages, ret, size;
	vm_offset_t addr = va;
	
//	kprintf("Calling Data Request ALL\n");
	vno->va = va;
	vno->page_size = PAGE_SIZE;
	msg.flags = MSG_LONG_SEND|MSG_MAP_RECEIVE;
	msg.snd_ptr = vno;
	msg.snd_bufsize = sizeof(struct vn_object);

	size = align_page(vno->size);
	msg.rcv_bufsize = size;
	msg.w1 = FS_DATA_REQ;
	msg.tid = vno->pager_id;
	msg.rcv_ptr = (void *)page_base(va);

	pages = msg.rcv_bufsize/PAGE_SIZE;
	do{
		page_base(addr);
		page = page_alloc(entry->object, addr);
//		kprintf("page %X ", page->base);
		pmap_enter(map->pmap, page->base, addr, PAGE_RW|PAGE_USER);
		addr += PAGE_SIZE;
	}while(--pages);

//	STI();
	ret = send_message(msg.tid, &msg);
	if(ret < 0)
	{
//		kprintf("Request to data server failed\n");
//		return -1;
	}
//	CLI();
//	if(vno->map_type & VM_MAP_ENTRY_FILE)
//		vno->offset += size;

	invlpg(NULL);
//	kprintf(" Fault Done\n");
	return 0;
	
}
/*
 * Request data from vnode from external pager
 */
int data_request(vm_map_entry_t entry, vm_offset_t va)
{
	struct message msg;
	vm_map_t map = current->proc->map;
	struct vn_object *vno = &entry->object->vn;
	struct vm_page *page;
	int pages, ret, size;
	vm_offset_t addr = va;

	kprintf("Faulted at %X XX", va);
	
	/* we don't need to accesss disk data for bss. */
	if((va >= vno->noload_start) && (va <= vno->noload_end))
	{
		kprintf("Zero filled in                    data_request: %X %X ", vno->noload_start,vno->noload_end);
		return alloc_zero_fill_page(map, entry, va);
	}	
	vno->va = va;
	vno->page_size = PAGE_SIZE;
	msg.flags = MSG_LONG_SEND|MSG_MAP_RECEIVE;
	msg.snd_ptr = vno;
	msg.snd_bufsize = sizeof(struct vn_object);

	if(vno->blk_size > PAGE_SIZE)
		size = vno->blk_size;
	else
		size = PAGE_SIZE;
	msg.rcv_bufsize = size;
	msg.w1 = FS_DATA_REQ;
	msg.tid = vno->pager_id;
	msg.rcv_ptr = (void *)page_base(va);

	pages = msg.rcv_bufsize/PAGE_SIZE;
	do{
		page_base(addr);
		page = page_alloc(entry->object, addr);
//		kprintf("page %X ", page->base);
		pmap_enter(map->pmap, page->base, addr, PAGE_RW|PAGE_USER);
		addr += PAGE_SIZE;
	}while(--pages);

//	STI();
	ret = send_message(msg.tid, &msg);
	if(ret < 0)
	{
//		kprintf("Request to data server failed\n");
//		return -1;
	}
//	CLI();
	if(vno->map_type & VM_MAP_ENTRY_FILE)
		vno->offset += size;

	invlpg(NULL);
	kprintf(" Fault Done\n");
	return 0;
}

