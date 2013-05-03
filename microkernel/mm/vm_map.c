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
 * File: vm_map.c
 *
 * This is the architecture independent part of the VM system.
 * The VM is based on Mach's VM Design.
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

#include <string.h>

#include <kernel/vm_map.h>
#include <kernel/vm_slab.h>
#include <kernel/pmap.h>
#include <kernel/kernel_stdio.h>
#include <kernel/scheduling.h>
#include <kernel/vm_object.h>
#include <kernel/list.h>

/* Get the last entry of the entry List. */
#define is_lastentry(head, entry)	(((entry)->next_entry == (head)) ? 1:0)

/* cache for vm_map. */
kmem_cache_t vm_map_cache = NULL;

/* cache for vm_map_entry. */
kmem_cache_t vm_map_entry_cache = NULL;

extern struct pmap kernel_map;
extern vm_offset_t start_va;

/* The kernel object. Composes of all kernel memory. */
extern struct vm_object kernel_object;

/* The first vm_map_entry_t for the Kernel. */
struct vm_map_entry kernel_entry = {
			{NULL,NULL},
			0,0,
			&kernel_object,0
			};

/* The vm_map for the kernel. */
struct vm_map kernel_vmmap = {
			&kernel_entry,
			&kernel_map,
			KERNEL_END-KERNEL_BASE,
			{0},
			0,
			KERNEL_BASE-1,
			NULL,
			};


/* Initialize the vm map. */
void vm_map_init()
{
	kernel_entry.start = align_page(start_va);
	kernel_entry.end = 0x1FFFFFFF;
	list_init(&kernel_entry.entries);
	SPIN_LOCK_INIT(&kernel_entry.lock);
	SPIN_LOCK_INIT(&kernel_vmmap.vmlock);

	vm_map_cache = kmem_cache_create("vm_map_cache", sizeof(struct vm_map), 8, NULL, NULL);
	if(!vm_map_cache)
		panic("vm_map_init: Not Enough Memory.");
	vm_map_entry_cache = kmem_cache_create("vm_entry_cache", sizeof(struct vm_map_entry),8, NULL,NULL);
	if(!vm_map_entry_cache)
		panic("vm_map_init: Not Enough Memory.");
}

/*
 * create a new vm_entry.
 * The Entry will be placed in the List just after the the nextentry.
 */
vm_map_entry_t create_vm_entry(vm_map_t map, vm_map_entry_t nextentry)
{
	vm_map_entry_t entry;
	entry = (vm_map_entry_t)kmem_cache_alloc(vm_map_entry_cache, 0);

	if(map->vmentry)
	{
		if(nextentry)
		{
			list_add_next(nextentry, entries, vm_map_entry_t, entry, entries, vm_map_entry_t);
		}
		else
		{
			list_add(&map->vmentry->entries, entry, entries, vm_map_entry_t);
		}

		return entry;
	}
	map->vmentry = entry;
	list_init(&entry->entries);

	return entry;
}

/*
 * make a copy of all the vm_entrys.
 * Needed to Duplicate Process (fork).
 */
void vm_entry_copy(vm_map_t new, vm_map_t old)
{
	vm_map_entry_t new_entry;
	vm_map_entry_t iterate, niterate = NULL;
	vm_object_t object;

	spin_lock(&old->vmlock);
	iterate = old->vmentry;

	do
	{
		new_entry = create_vm_entry(new, niterate);
		new_entry->start = iterate->start;
		new_entry->end = iterate->end;
		new_entry->object = iterate->object;
		new_entry->type = iterate->type;
		new_entry->attr = iterate->attr;

		object = new_entry->object;

		/* We do now cow shadow objects */
/*		while(object->shadow)
			object = object->shadow;
*/
//		if(!(object->attribute & COW_IN_PROGRESS))
		/* COW the pages held by src.*/
//			pmap_cow(old->pmap, new_entry->start, new_entry->end);

		object->attribute |= COPY_ON_WRITE|COW_IN_PROGRESS;
		atomic_inc(&object->ref_count);

		new_entry->object = object;

		iterate = iterate->next_entry;
		niterate = new_entry;
	}while(iterate != (vm_map_entry_t)&old->vmentry->entries);

	iterate = old->vmentry;
	do
	{
		object = iterate->object;

		/* We do now cow shadow objects */
		while(object->shadow)
			object = object->shadow;

		object->attribute &= ~COW_IN_PROGRESS;
		iterate = iterate->next_entry;
	}while(iterate != (vm_map_entry_t)&old->vmentry->entries);
	spin_unlock(&old->vmlock);
}

/*
 * Create a new vm map structure.
 */
vm_map_t vm_map_create()
{
	vm_map_t map;

	map = (vm_map_t) kmem_cache_alloc(vm_map_cache, 0);
	map->start = 0;
	map->end = KERNEL_BASE;
	atomic_set(&map->ref_count, 1);
	SPIN_LOCK_INIT(&map->vmlock);

	return map;
}

/*
 * Destroy the vm map.
 */
void vm_map_destroy(vm_map_t map)
{
	if(atomic_get(&map->ref_count) == 0)
		kmem_cache_free(vm_map_cache, map);
}

/*
 * Duplicate the given map according to the given flag.
 * Duplicate the whole map including pages. Used by fork.
 */
vm_map_t vm_map_fork(vm_map_t map, u32 flag)
{
	vm_map_t new_vm_map;

	new_vm_map = vm_map_create();
	if(!new_vm_map)
		return NULL;

	new_vm_map->start = 0x200000;	/* We dont touch Lower Memory. */
	new_vm_map->end = KERNEL_BASE-1;
	new_vm_map->hint = NULL;

	/* Copy every thing. But COW sharing is done for data. */
	if(flag & DUP_ALL)
	{
		new_vm_map->pmap = NULL;
		new_vm_map->vmentry = NULL;
		vm_entry_copy(new_vm_map, map);
		new_vm_map->pmap = pmap_copy(map->pmap);

		return new_vm_map;
	}
	else if(flag & DUP_NONE)
	{
		new_vm_map->vmentry = NULL;
		new_vm_map->size = KERNEL_BASE;
//		atomic_inc(&new_vm_map->ref_count);
		new_vm_map->pmap = pmap_create();
		pmap_share_kernel(new_vm_map->pmap);
	}

	return new_vm_map;
}


/*
 * Clip the map entry
 */
vm_map_entry_t vm_map_entry_clip(vm_map_t map, vm_map_entry_t entry, vm_offset_t start)
{
	vm_map_entry_t nextentry;

	if(start == entry->start)
		return entry;

	if((start > entry->start) && (start < entry->end))
	{
		nextentry = create_vm_entry(map, entry);
		*nextentry = *entry;
		atomic_inc(&entry->object->ref_count);
		nextentry->start = start;
		entry->end = start;

		return nextentry;
	}

	return NULL;
}

/*
 * Create a new entry in the given map.
 */
int vm_map_enter(vm_map_t map, vm_offset_t *address, vm_size_t size, vm_object_t object, int attribute)
{
	vm_map_entry_t entry, newentry = NULL,next;
	vm_object_t obj = object;
	vm_offset_t start = *address, end = *address + size;
	int ret = -ENOMEM;

	spin_lock(&map->vmlock);

	if(map->vmentry == NULL)
		goto createntry;

	newentry = vm_map_lookup_entry(map, *address);
	if((newentry != NULL) && (attribute & VM_MAP_ENTRY_OBJECT_FIXED))
	{
		kprintf("VM_MAP_ENTER: Failed for fixed Object.");
		return -EINVAL;
	}

	if(newentry)
	{
		start = newentry->end;
		end = start + size;

	}
	else
		goto handle_fixed;

	do{
		next = newentry->next_entry;
		if(next == map->vmentry)
			goto confirm;

		if(end < next->start)
		{
confirm:
			/* We can combine the two vm_entry */
			if ((object == NULL) &&
				(newentry->type & attribute))
			{
				start = newentry->end;
				newentry->end += size;
				*address = start;
				spin_unlock(&map->vmlock);
//				kprintf("XX start %x end %x ", start, newentry->end);
				return 0;
			}

			start = newentry->end;
			end = start + size;
			break;

		}
		start = newentry->end;
		end = start + size;
		newentry = next;
	}while(next != map->vmentry);

createntry:
	if(end > map->end)
		goto err;

	/* Now, create a new entry. */
	entry = create_vm_entry(map, newentry);
	if(!entry)
		kprintf(" create entry failed.");
	entry->object = obj;
	entry->start = start;
	entry->end = end;
	entry->type = attribute;
	*address = start;
	spin_unlock(&map->vmlock);
//	kprintf(" start %x end %x ", start, end);
	return 0;

handle_fixed:
	/* Check if the address space is available for map which needs fixed va */
	/* if not return error. */
//	kprintf("Fixed Map");
	entry = map->vmentry;
	do
	{
		if(end > entry->start && end < entry->end)
			return -ENOMEM;
		if(start < entry->start)
		{
			if(end > entry->start)
			{
				ret = -ENOMEM;
				goto err;
			}
			break;
		}
		entry = entry->next_entry;
	}while(entry != map->vmentry);
	newentry = entry;
	goto createntry;
err:
	spin_unlock(&map->vmlock);
	kprintf("err %X %d ", *address, size);
	return ret;
}

static inline void dealloc_vm_map_entry(vm_map_t map, vm_map_entry_t entry)
{
	list_del(&map->vmentry->entries, entry, entries, vm_map_entry_t);
	kmem_cache_free(vm_map_entry_cache, entry);

}

/* delete the vm_map_entry */
int delete_vm_map_entry(vm_map_t map, vm_map_entry_t entry)
{
	delete_object(entry->object);

	dealloc_vm_map_entry(map, entry);
	return 0;
}



/* Remove the address from the map */
int vm_map_remove(vm_map_t map, vm_offset_t offset, size_t size)
{
	vm_map_entry_t entry, newentry, nextentry;
	int ret = 0;
//	vm_object_t object;

	spin_lock(&map->vmlock);
	entry = vm_map_lookup_entry(map, offset);
	if(entry == NULL)
	{
		ret = -1;
		goto end;
	}

	newentry = vm_map_entry_clip(map, entry, offset);
	if(!newentry)
	{
		ret = -1;
		goto end;
	}

	nextentry = vm_map_entry_clip(map, newentry, offset + size);
	delete_vm_map_entry(map, newentry);

end:
	spin_unlock(&map->vmlock);
	return ret;
}


/* lock all the pages of the map */
int vm_lock_all(vm_map_t map)
{
	vm_map_entry_t entry;

	spin_lock(&map->vmlock);
	entry = map->vmentry;
	for(;entry != map->vmentry ; entry = entry->next_entry)
	{
		entry->attr |= VM_MAP_ENTRY_LOCKED;
		vm_lock_object(entry);
	}
	spin_unlock(&map->vmlock);

	return 0;
}

/* Unlock all the pages */
int vm_unlock_all(vm_map_t map)
{
	vm_map_entry_t entry;

	spin_lock(&map->vmlock);
	entry = map->vmentry;
	for(;entry != map->vmentry ; entry = entry->next_entry)
	{
		entry->attr &= ~VM_MAP_ENTRY_LOCKED;
		vm_unlock_object(entry);
	}
	spin_unlock(&map->vmlock);

	return 0;

}



/* Lock the pages in memory */
int vm_lock(vm_map_t map, vm_offset_t offset, vm_size_t size, int flag)
{
	vm_map_entry_t entry, newentry = NULL, nextentry;
	int ret = 0;

	if(!superuser())
		return -EPERM;

	if(flag)
	{
		map->attr = flag;
		vm_lock_all(map);
		return 0;
	}

	spin_lock(&map->vmlock);
	entry = vm_map_lookup_entry(map, offset);
	if(!entry)
	{
		ret = -ENOMEM;
		goto end;
	}

	newentry = vm_map_entry_clip(map, entry, offset);
	if(!newentry)
	{
		ret = -ENOMEM;
		goto end;
	}

	nextentry = vm_map_entry_clip(map, newentry, offset + size);

	newentry->attr |= VM_MAP_ENTRY_LOCKED;
	vm_lock_object(entry);
end:
	spin_unlock(&map->vmlock);
	return ret;
}

/* Unlock the pages */
int vm_unlock(vm_map_t map, vm_offset_t offset, vm_size_t size, int flag)
{
	vm_map_entry_t entry, newentry = NULL, nextentry;
	int ret = 0;

	if(!superuser())
		return -EPERM;

	if(flag)
	{
		map->attr &= ~VM_LOCK;
		vm_unlock_all(map);
		return 0;
	}

	spin_lock(&map->vmlock);
	entry = vm_map_lookup_entry(map, offset);
	if(!entry)
	{
		ret = -ENOMEM;
		goto end;
	}

	newentry = vm_map_entry_clip(map, entry, offset);
	if(!newentry)
	{
		ret = -ENOMEM;
		goto end;
	}

	nextentry = vm_map_entry_clip(map, newentry, offset + size);

	newentry->attr &= ~VM_MAP_ENTRY_LOCKED;
	vm_unlock_object(entry);

end:
	spin_unlock(&map->vmlock);
	return ret;

}

/* Change the protection the memory range */
int vm_protect(vm_map_t map, vm_offset_t offset, vm_size_t size, int prot)
{
	vm_map_entry_t entry, newentry, nextentry;
	int ret = 0;

	spin_lock(&map->vmlock);
	entry = vm_map_lookup_entry(map, offset);
	if(!entry)
	{
		ret = -ENOMEM;
		goto end;
	}

	newentry = vm_map_entry_clip(map, entry, offset);
	if(!newentry)
	{
		ret = -ENOMEM;
		goto end;
	}

	nextentry = vm_map_entry_clip(map, newentry, offset + size);

	newentry->attr |= prot;
	pmap_protect(map->pmap, offset, offset+size, prot);

end:
	spin_unlock(&map->vmlock);
	return ret;
}

/* Find the entry by the address. */
vm_map_entry_t vm_map_lookup_entry(vm_map_t map, vm_offset_t address)
{
	vm_map_entry_t tmp;

	if(!map || !map->vmentry)
		return NULL;

	if(map->hint != NULL)
	{
		if((address >= map->hint->start) && (address <= map->hint->end))
			return map->hint;
	}

	for(tmp = map->vmentry; ; tmp = tmp->next_entry)
	{
		if(tmp == NULL)
			return NULL;

		if((address >= tmp->start) && (address <= tmp->end))
		{
			map->hint = tmp;
			return tmp;
		}

		if(tmp->next_entry == map->vmentry)
			break;

	}

	return NULL;
}

/* Get the free virtual memory in the given map */
vm_offset_t get_vaddr(vm_map_t map, vm_size_t size)
{
	vm_map_entry_t mapentry = map->vmentry, nextentry;
	vm_offset_t va, end;

	spin_lock(&map->vmlock);
again:
	end = mapentry->end + size;
	nextentry = (vm_map_entry_t)mapentry->next_entry;
	if((end < nextentry->start) || (is_lastentry(map->vmentry, mapentry)))
	{
//		kprintf("here");
		if(end > map->end)
		{
			spin_unlock(&map->vmlock);
			return 0;
		}
		va = mapentry->end;
		mapentry->end = end;
	}
	else
	{
		mapentry = (vm_map_entry_t)mapentry->next_entry;
		goto again;
	}

	spin_unlock(&map->vmlock);
	return va;
}

/* Free the virtual address from the given map of size "size" */
int free_vaddr(vm_map_t map, vm_offset_t va, vm_size_t size)
{
	vm_map_entry_t entry, newentry, nextentry;
	int ret = 1;

	spin_lock(&map->vmlock);
	entry = vm_map_lookup_entry(map, va);
	if(!entry)
	{
		ret = 0;
		goto end;
	}

	newentry = vm_map_entry_clip(map, entry, va);
	if(!newentry)
	{
		ret = 0;
		goto end;
	}

	nextentry = vm_map_entry_clip(map, newentry, va + size);
end:
	spin_unlock(&map->vmlock);
	return 1;
}

/* Map the address ptr of snd to the map rcv  */
int vm_share_addr(vm_map_t rcv, vm_map_t snd, vm_offset_t *ptr, size_t size, int flag)
{
	vm_map_entry_t entry;
	vm_page_t page;
	vm_offset_t va = *ptr, alignedva,  nextva;
	int times, ret = 0;

	times = align_page(size)/PAGE_SIZE;
	alignedva = va;
	page_base(alignedva);
	alignedva = va - alignedva;
//	kprintf("ptr is %x %d size %d times", va, size, times);
	vm_allocate(rcv, ptr, size);
	nextva = *ptr;
//	kprintf("new ptr is %x ", nextva);
	spin_lock(&snd->vmlock);
	do{
//		kprintf("Base is ");
		entry = vm_map_lookup_entry(snd, va);
		if(entry == NULL)
		{
			kprintf("vm_share_addr: MAP lookup returned NULL for snd %X\n", va);
			ret = -1;
			goto end;
		}
//		kprintf(" %d ", times);
		page_base(va);
		page = vm_page_lookup(entry->object, va);
		if(page == NULL)
		{
			kprintf("vm_share_addr: page lookup returned NULL for %X\n", va);
			ret = -1;
			goto end;
		}
//		kprintf(" %x ", nextva);
		pmap_enter(rcv->pmap, page->base, nextva, flag);
		nextva += PAGE_SIZE;
		va += PAGE_SIZE;
	}while(--times);

	spin_unlock(&snd->vmlock);
	*ptr = (*ptr) + alignedva ;
//	kprintf("final ptr is %x ", *ptr);
end:
	return ret;
}

/* Unmap the address */
int vm_unshare_addr(vm_map_t target, vm_offset_t offset, size_t size)
{
	vm_deallocate(target, offset, size);
	pmap_remove(target->pmap, offset, offset+size);

	return 0;

}

/* Destroy the vmmap. Called by exit. */
void exit_vm_map(vm_map_t vmmap)
{
	vm_map_entry_t entry = vmmap->vmentry;
	vm_object_t object, next_object;

	if(atomic_dec(&vmmap->ref_count) > 0)
	{
		kprintf("exitvmmap atomic dec not zero ");
		return;
	}
	if(!entry)
		return;

	spin_lock(&vmmap->vmlock);
	while(entry != vmmap->vmentry)
	{
		object = entry->object;
		do{
			next_object = object->shadow;
//			delete_object(object);
			object = next_object;
		}while(next_object);
		dealloc_vm_map_entry(vmmap, entry);

		entry = entry->next_entry;
	}
	spin_unlock(&vmmap->vmlock);
}

/*
 * This Function Handles the Copy on Write Feature
 */
int handle_cow(vm_map_t map, vm_map_entry_t entry, vm_offset_t offset)
{
	vm_object_t object, shadow;
	vm_page_t page, page2;
	vm_offset_t addr = page_base(offset);

	object = entry->object;

	while(object->shadow)
		object = object->shadow;

	page = vm_page_lookup(object, addr);
	if(!page)
		return -1;

	/* Just set the RW bit if we are the only writers */
	if(atomic_get(&object->ref_count) == 1)
		pmap_protect(map->pmap, addr, addr+PAGE_SIZE, VM_PAGE_WRITE);

	shadow = entry->object;
	if(!(shadow->attribute & OBJECT_SHADOW))
		/* Create a shadow object */
		shadow = vm_object_create(PAGE_SIZE);

	page2 = page_alloc(shadow, addr);
	pmap_enter(map->pmap, page2->base, addr, PAGE_RW|PAGE_USER);

	pmap_enter(map->pmap, page->base, page->base+KERNEL_BASE, PAGE_RW|PAGE_USER);
	ker_to_usr((void *)addr, (void *)page->base, PAGE_SIZE);
//	memcpy((char *), (char *)page->base, PAGE_SIZE);	//FIXME:
	pmap_remove(map->pmap, page->base+KERNEL_BASE, page->base + PAGE_SIZE + KERNEL_BASE);

	return 0;
}

