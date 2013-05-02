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
 * File: pmap.c
 *
 * This is the architecture dependent part of the VM system. The Virtual Memory
 * manager is based on Mach's VM design.
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

#include <string.h>

#include <kernel/kernel_stdio.h>
#include <kernel/vm.h>
#include <kernel/atomic.h>
#include <kernel/spinlock.h>
#include <kernel/info.h>

#include <asm/x86.h>
#include <asm/mm.h>
#include <asm/pmap.h>

/* The starting Physical address */
extern vm_offset_t starting_mem;

/* The ending physical address */
extern vm_offset_t ending_mem;

/* The vm_map for the kernel. */
extern struct vm_map kernel_vmmap;

vm_offset_t start_va = 0;

struct pmap kernel_map = {{0}};

struct vm_object pmap_object =	{
				{NULL, NULL},
				};

/* Caches for pmap */
kmem_cache_t pmap_cache = NULL;

/*
 * Initialize pmap before slab.
 * We initialize mapping for 1st 4mb only.
 */
void pmap_pre_init()
{
	extern u8 _text_start[], _end[], _text_end[];
	vm_offset_t addr;
	int i;
	pde_t *pte;
	pde_t *pde;

//	SPIN_LOCK_INIT(&kernel_map.lock);
//	SPIN_LOCK_INIT(&pmap_object.lock);
	kernel_map.pdbr = (pde_t *) get_next_frame();

	pte = (pde_t *) get_next_frame();

	pde = kernel_map.pdbr;

	for(i=0 ; i < NTABLES ; i++)
		kernel_map.pdbr[i] = 0;

	/*  we initialize one page directory entries only. It is
	 * enough to hold 4 MB memory.
	 */
	pde[addr_to_pdenum(KERNEL_BASE)] = (addr_to_pte((vm_offset_t)pte)) | PAGE_PRESENT | PAGE_RW;
	/* temp mapping only */
	pde[0] = (addr_to_pte((vm_offset_t)pte)) | PAGE_PRESENT | PAGE_RW ;

	for(i = 0 ; i < NTABLES ; i++)
	{
		addr = i*PAGE_SIZE ;

		if (addr >= 0xa0000 && addr <= 0xbffff)
			pte[i] = (addr_to_pte(addr)) | PAGE_PRESENT | PAGE_RW ;

		if((addr >= (vm_offset_t)_text_start) && (addr <= (vm_offset_t)_end))
		{
			if((addr >= (vm_offset_t)_text_start) && (addr <= (vm_offset_t)_text_end))
				pte[i] = (addr_to_pte(addr)) | PAGE_PRESENT|PAGE_RW;

			else
				pte[i] = (addr_to_pte(addr)) | PAGE_PRESENT | PAGE_RW;
		}
		else if((addr >= (vm_offset_t)pte) && (addr <= ((vm_offset_t)pte + PAGE_SIZE)))
			pte[i] = (addr_to_pte(addr)) | PAGE_PRESENT | PAGE_RW ;

		else if((addr >= (vm_offset_t)pde) && (addr <= ((vm_offset_t)pde + PAGE_SIZE)))
			pte[i] = (addr_to_pte(addr)) | PAGE_PRESENT | PAGE_RW ;

		else
			pte[i] = 0 /*(addr_to_pte(addr)) | PAGE_PRESENT | PAGE_RW */;
	}
	invlpg((void *)addr);
	list_init(&pmap_object.page);
	pmap_object.size = ending_mem-starting_mem;

}

/* Initialize the pmap */
void pmap_init()
{
	pmap_cache = kmem_cache_create("pmap_cache", sizeof(struct pmap), 4, pmap_const, NULL);
	if(!pmap_cache)
		panic("pmap_init: Not Enough Memory.");

}

/* returns the available physical memory */
u32 get_mem_free()
{
	return (get_avail_mem());
}

u32 get_total_mem()
{
	return ending_mem;
}


/* Allocate physical memory of given no of pages */
vm_offset_t alloc_physical_mem(u32 npages)
{
	vm_offset_t addr, add;
	int i;

	addr = get_next_frame();
	pmap_enter(&kernel_map, addr, (KERNEL_BASE+addr), PAGE_RW);
	add = addr;

	for(i=1; i<npages ;i++)
	{
		add = get_next_frame();
		pmap_enter(&kernel_map, add, (KERNEL_BASE+add), PAGE_RW);
	}
	start_va = add + PAGE_SIZE;
	return (addr);
}

/* Allocate a page to be used by page tables */
vm_offset_t page_table_allocate(pmap_t pmap)
{
	vm_page_t page;

	page = get_page(PAGE_ALLOC_KER);

	atomic_set(&page->count, 1);
	vm_page_insert(page, &pmap_object, page->base);
	list_add(&pmap->plist, page, pagelist, vm_page_t);

	pmap_enter(&kernel_map, page->base, page->base+KERNEL_BASE, PAGE_RW);
	memset((char *)page->base, 0, PAGE_SIZE);

	spin_unlock(&page->lock);
	return page->base;
}

/*
 * Update the page directory entry of all the process' kernel page table area.
 */
static inline void kernel_map_update(vm_offset_t pte, vm_offset_t idx)
{
	process_t p;
	pmap_t map;

	spin_lock(&syslock);
	p = (process_t) sysinfo.proclist.next;
	for(; p != (process_t)&sysinfo.proclist; p = p->proclist.next)
	{
		map = p->map->pmap;
		map->pdbr[idx] = pte;
	}
	spin_unlock(&syslock);
}

/* Map the page paddr to virtual address vaddr */
void pmap_enter(pmap_t map, vm_offset_t paddr, vm_offset_t vaddr, u32 protection)
{
	register pde_t *pte;
	register pde_t *pde;
	vm_offset_t newpte, idx;

	spin_lock(&map->lock);
	if(map->pdbr == NULL)
	{
		pde = (pde_t *)page_table_allocate(map);
		map->pdbr = pde;
	}
	else
		pde = map->pdbr;

	/* Check if we have to create a new page table entry */
	if(pde[addr_to_pdenum(vaddr)] == 0)
	{
		pte = (pde_t *)page_table_allocate(map);
		newpte = (addr_to_pte((vm_offset_t)pte)) |PAGE_PRESENT| protection;
		idx = addr_to_pdenum(vaddr);
		if(map == &kernel_map)
			kernel_map_update(newpte, idx);
		pde[idx] = newpte;
	}
	else
		pte = (pde_t *)addr_to_pte(pde[addr_to_pdenum(vaddr)]);

	pte[addr_to_ptenum(vaddr)] = ((vm_offset_t)addr_to_pte(paddr)) | PAGE_PRESENT | protection;

	spin_unlock(&map->lock);
	invlpg((void *)vaddr);
}

/* Map the range of pages to virtual address vaddr */
void pmap_enter_range(pmap_t map, vm_offset_t paddr, vm_offset_t vaddr, vm_size_t limit, int protection)
{
	vm_offset_t pa;
	vm_size_t size = align_page(limit);

	for(pa = paddr ; pa <= (paddr + size);)
	{
		pmap_enter(map, pa, vaddr, protection);
		pa += PAGE_SIZE;
		vaddr += PAGE_SIZE;
	}
}


/* Create a new PMAP and return its address */
pmap_t pmap_create()
{
	pmap_t pmap;
	pmap = kmem_cache_alloc(pmap_cache, 0);
	list_init(&pmap->plist);
	SPIN_LOCK_INIT(&pmap->lock);
	return pmap;
}

static inline void pmap_page_free(pmap_t pmap)
{
	vm_page_t page;

	spin_lock(&pmap->lock);
	while(!list_isempty(&pmap->plist))
	{
		list_remove(&pmap->plist, page, pagelist, vm_page_t);
		page_dealloc(page->object, page->offset);
	}
	spin_unlock(&pmap->lock);
}

/* Destroy the pmap */
int pmap_destroy(pmap_t map)
{
	pmap_page_free(map);
	kmem_cache_free(pmap_cache, map);

	return 0;
}

/* Make a copy of the pmap src
 * It is used by fork.
 */
pmap_t pmap_copy(pmap_t src)
{
	pmap_t map;
	pde_t *src_pde, *src_pte, *dest_pde, *dest_pte;
	int pde, pte;

	map = pmap_create();
	list_init(&map->plist);
	SPIN_LOCK_INIT(&map->lock);

	dest_pde = map->pdbr = (pde_t *)page_table_allocate(map);

	spin_lock(&src->lock);
	src_pde = src->pdbr;

	/* Copy every used page tables.*/
	for(pde = 0; pde < KTABLE_START; pde++)
	{
		if(!src_pde[pde])
			continue;

		dest_pte = (pde_t *)page_table_allocate(map);
		src_pte = (pde_t *)addr_to_pte(src_pde[pde]);
		for(pte=0; pte<NTABLES; pte++)
		{
			if(!src_pte[pte])
				continue;
			dest_pte[pte] = src_pte[pte];
		}

		dest_pde[pde] = (addr_to_pte(dest_pte)) | ((src_pde[pde]) & (0xFFF));
	}
	spin_unlock(&src->lock);
	spin_lock(&kernel_map.lock);
	/* Share the kernel tables */
	src_pde = kernel_map.pdbr;

	for(pde = KTABLE_START; pde < NTABLES; pde++)
	{
		if(!src_pde[pde])
			continue;
		dest_pde[pde] = src_pde[pde];
	}

	spin_unlock(&kernel_map.lock);
	invlpg(NULL);
	return map;
}

/* share the pamp of the kernel with the new process */
void pmap_share_kernel(pmap_t nmap)
{
	pde_t *dest, *src;
	int pde;

	nmap->pdbr = (pde_t *)page_table_allocate(nmap);
	dest = nmap->pdbr;

	spin_lock(&kernel_map.lock);
	src = kernel_map.pdbr;

	for(pde = KTABLE_START; pde < NTABLES; pde++)
	{
		if(!src[pde])
			continue;
		dest[pde] = src[pde];
	}

	spin_unlock(&kernel_map.lock);
	invlpg(NULL);
}

void exec_pmap(pmap_t map)
{
	pmap_page_free(map);
	memset(map->pdbr, 0, KTABLE_START * sizeof(pde_t *));
}


/* Remove the mapping for the given virtual address */
void pmap_remove(pmap_t map, vm_offset_t svaddr, vm_offset_t evaddr)
{
	register pde_t *pde, *pte;
	if(map->pdbr == NULL)
		return;

	spin_lock(&map->lock);
	pde = map->pdbr;

	while(svaddr < evaddr)
	{
		pte = (pde_t *)pde[addr_to_pdenum(svaddr)];
		pte[addr_to_ptenum(svaddr)] = 0;
		invlpg((void *)svaddr);
		svaddr += PAGE_SIZE;
	}
	spin_unlock(&map->lock);
}

/* Change the protection bits of the given pmap entries */
int pmap_protect(pmap_t map, vm_offset_t svaddr, vm_offset_t evaddr, int prot)
{
	pde_t *pde, *pte;

	if(map->pdbr == NULL)
		return 0;

	spin_lock(&map->lock);
	pde = map->pdbr;

	while(svaddr < evaddr)
	{
		pte = (pde_t *)addr_to_pte(pde[addr_to_pdenum(svaddr)]);

		/* Ia32 do not have execute bit. so we assume read = execute */
		if((prot & VM_PAGE_READ) || (prot & VM_PAGE_EXEC))
			pte[addr_to_ptenum(svaddr)] &= ~PAGE_RW;
		if(prot & VM_PAGE_WRITE)
			pte[addr_to_ptenum(svaddr)] |= PAGE_RW;
		svaddr += PAGE_SIZE;
	}

	spin_lock(&map->lock);
	invlpg((void *)svaddr);
	return 1;
}

/* Make all the pages in the range Read Only */
void pmap_cow(pmap_t map, vm_offset_t start, vm_offset_t end)
{
	pde_t *pde, *pte;

	spin_lock(&map->lock);
 	if(!map->pdbr)
		goto end;

 	pde = map->pdbr;

 	while(start < end)
	{
		pte = (pde_t *)addr_to_pte(pde[addr_to_pdenum(start)]);
		pte[addr_to_ptenum(start)] &= ~PAGE_RW;
		start += PAGE_SIZE;
		invlpg((void *)start);
	}
end:
	spin_unlock(&map->lock);
 }

/*
 * Constructor For Cache for pmap.
 */
void pmap_const(void *pmap, kmem_cache_t cache)
{
	pmap_t mapc;
	mapc = (pmap_t) pmap;
	mapc->pdbr = NULL;
}
