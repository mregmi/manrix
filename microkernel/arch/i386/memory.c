/**
 * ManRiX Microkernel
 * Copyright (C) 2004, 2005
 * 	Manish Regmi (regmi dot manish at gmail dot com) and
 * 	Rajesh R.C   (rajesh dot rc at gmail dot com)
 *
 * This Program is free software; you can redistribute it and/or modify	\
 * it under the terms of the GNU General Public License as published by	\
 * the Free Software Foundation; either version 2, or (at your option)	\
 * any later version.
 *
 * This Program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 **/
/**
 * File:- memory.c
 *	The I386 specfic Memory Management Routines.
 **/
#include <string.h>
#include <kernel/config.h>
#include <kernel/kernel_stdio.h>
#include <kernel/vm.h>
#include <kernel/scheduling.h>
#include <kernel/info.h>

#include <asm/multiboot.h>
#include <asm/descriptors.h>
#include <asm/x86.h>
#include <asm/asm.h>
#include <asm/pmap.h>

extern struct pmap kernel_map;
extern struct vm_map kernel_vmmap;
extern struct vm_object kernel_object;
extern struct vm_object pmap_object;

/*
 * The information passed by BootLoader
 */
extern struct multiboot_info *minfo;

/*
 * The Ending Memory Address of the Kernel Image
 */
extern int _end[];

/*
 * The starting Physical address
 */
vm_offset_t starting_mem = 0 ;

/*
 * The ending physical address
 */
vm_offset_t ending_mem = 0;

/* Total available memory */
vm_offset_t available_mem = 0;

/*
 * starting address of next frame
 * We wont be using the lower 1 m.
 * The first frame always starts from the end
 * of the kernel but skips the modules image.
 */
static vm_offset_t next_frame = (vm_offset_t)_end;
static vm_offset_t mod_start=0, mod_end =0, cmdstart, cmdend;

extern struct pmap kernel_map;
extern struct sysinfo sysinfo;

/*
 * Initialize The Physical memory
 */
void init_memory()
{
	starting_mem = 0;
	ending_mem = 0x100000 + (minfo->mem_upper) * 1024;
	available_mem = (minfo->mem_upper * 1024) + (minfo->mem_lower * 1024);
	kprintf("Total Memory: %dK\n",ending_mem/1024);

	/* Update The Sysinfo */
	sysinfo.total_mem = sysinfo.total_ram = ending_mem;

	mod_start = minfo->mods_addr;
	mod_end = (vm_offset_t)(minfo->mods_addr + (minfo->mods_count * sizeof(struct module)));

	cmdstart = minfo->cmdline;
	cmdend = strlen((char *)cmdstart) + cmdstart;
}

/*
 * Get the next free Page frame.
 * We Skip the frames already used by Modules and
 * Multiboot information.
 * 	Args: None
 * 	Output: Returns the Page Frame Base Address
 */
vm_offset_t get_next_frame()
{
	int i;
	vm_offset_t addr, nextend;
	int modcmd, modcmdend;

retry:
	align_page(next_frame);
	nextend = next_frame + PAGE_SIZE;

	/*
	 * Ok, All of the Available Memory is Used.
	 */
	if(next_frame >= ending_mem)
		return 0;

	/*
	 * Skip the Module Structures.
	 */
	if((nextend > mod_start) && (next_frame < mod_end))
	{
		next_frame = (vm_offset_t)mod_end;
		goto retry;
	}

	/*
	 * Skip the Command Line Args to the Kernel.
	 */
	if((nextend > cmdstart) && (next_frame < cmdend))
	{
		next_frame = (vm_offset_t)cmdend;
		goto retry;
	}

	/*
	 * Skip the Module Images.
	 */
	for(i = 0 ; i< minfo->mods_count ; i++)
	{
		module_t m = (module_t)(minfo->mods_addr + (i * sizeof(struct module)));

		modcmd = m->string;
		modcmdend = strlen((char *)modcmd) + modcmd;

		if((nextend > modcmd) && (next_frame < modcmdend))
		{
			next_frame = (vm_offset_t)modcmdend;
			goto retry;
		}

		if((nextend > m->mod_start) && (next_frame < m->mod_end))
		{
			next_frame = (vm_offset_t)m->mod_end;
			goto retry;
		}
	}

	addr = next_frame;
	next_frame += PAGE_SIZE;
	available_mem -= PAGE_SIZE;

	return addr;
}

/*
 * Get the total Memory Available in System
 *	Arg: None
 *	Outputs: Available Memory in Bytes.
 * Note: the Memory in Free Page List is not Avalilable Memory.
 */
u32 get_avail_mem()
{
	return (available_mem);
}


/*
 * Page fault handling routine.
 * 	Args: Registers in the Stack
 */
void do_page_fault( struct stack_regs *regs)
{
	vm_offset_t address;
	vm_map_entry_t entry;
	vm_map_t map;
	int ret = 0;
	thread_t thr = current;
	u32 error_code = regs->error_code;

	address = read_cr2();

//	kprintf("page fault %X %X pid %d tid %d ", thr, address, thr->proc->pid, thr->tid);
	spin_lock(&thr->proc->map->vmlock);
	map = thr->proc->map;

	/*
	 * Find if the address Exists in the vm_map.
	 * If it does not exist, send SEGV
	 */
	entry = vm_map_lookup_entry(map, address);
	if(!entry)
	{
		kprintf("ENTRY returned NULL\n");
		goto fault;
	}

	/*
	 * Check if it is zero fill memory.
	 * If yes allocate a page and map with va.
	 */
	if(entry->type & VM_MAP_ENTRY_ZERO_FILL)
		goto handle_zero_fill;

	if(entry->type & VM_MAP_ENTRY_ANON)
		goto handle_anon;

	/* page not present */
	if(!(error_code & 1))
	{
//		kprintf("Data Request by tid %d pid %d", thr->tid, thr->proc->pid);
		spin_unlock(&map->vmlock);
		ret = data_request(entry, address);
		if(ret < 0)
			goto fault;

		return;
	}

	/*
	 * If the Fault is due to Write.
	 */
	if(error_code & 2)
	{
		kprintf("Cow handled ");
		ret = handle_cow(thr->proc->map, entry, address);
		spin_unlock(&map->vmlock);
		return;
	}

handle_zero_fill:
	spin_unlock(&map->vmlock);
	alloc_zero_fill_page(map, entry, address);
	
	return;
handle_anon:
	// FIXME
fault:
	spin_unlock(&map->vmlock);
	kprintf("Sending SEGV to thread %d add %x\n", thr->tid, address);
	print_regs(regs);
//	panic("ERROR:");
	send_signal(thr, SIGSEGV, NULL);
//	schedule();
//	panic("PANIC");
}

/*
 * syscall for vm_map
 * 	Args:
 * 	ecx = pointer to vn_object
 * 	edx = map type
 */
void *syscall_vm_map(struct stack_regs *regs)
{
	process_t p = current->proc;
	struct vn_object vno, *vn = NULL;
	void *ptr = NULL;

	if(regs->ebx)
	{
		vn = &vno;
		usr_to_ker((void *)&vno, (void *)regs->ebx, sizeof(struct vn_object));
	}

	if(regs->ecx & VM_MAP_ENTRY_LOWMEM)
	{
		if(p->map->private == 0)
			p->map->private = 0x1000;	/* Start from page 1 */

		pmap_enter_range(p->map->pmap, p->map->private, p->map->private, vn->size, PAGE_RW|PAGE_USER);
		ptr = (void *)p->map->private;
		p->map->private += vn->size;
	}
	else
	{
		ptr = vm_map(p->map, vn, regs->ecx);
	}

	return ptr;
}

/*
 * syscall for vm_allocate. Allocates zero filled memory
 * 	Args:
 * 	ebx = address
 * 	ecx = size
 */
int syscall_vm_allocate(struct stack_regs *regs)
{
	vm_map_t map = current->proc->map;

	return vm_allocate(map, &regs->ebx, regs->ecx);
}

/*
 * syscall for vm_lock. locks the memory i.e
 * it cannot be paged.
 * 	Args:
 * 	ebx = address
 * 	ecx = size
 * 	edx = flag
 */
int syscall_vm_lock(struct stack_regs *regs)
{
	process_t p = current->proc;

	return vm_lock(p->map, regs->ebx, regs->ecx, regs->edx);
}

/*
 * syscall for vm_unlock. unlocks the memory i.e
 * it can be paged.
 *	Args:
 * 	ebx = address
 * 	ecx = size
 * 	edx = flag
 */
int syscall_vm_unlock(struct stack_regs *regs)
{
	process_t p = current->proc;

	return vm_unlock(p->map, regs->ebx, regs->ecx, regs->edx);
}

/*
 * syscall for changing the protection of the memory
 * 	Args:
 * 	ebx = address
 * 	ecx = size
 * 	edx = protection
 */
int syscall_vm_protect(struct stack_regs *regs)
{
	process_t p = current->proc;

	return vm_protect(p->map, regs->ebx, regs->ecx, regs->edx);
}


/*
 * syscall for vm_deallocate. deallocates memory region
 * 	Args:
 *  ebx = address
 *  ecx = size
 */
int syscall_vm_deallocate(struct stack_regs *regs)
{
	process_t p = current->proc;

	return vm_deallocate(p->map, (vm_offset_t)&regs->ebx, regs->ecx);
}

/*
 * Initialize the paging System.
 * We first update the CR3 Register and
 * set the PE bit in CR0.
 */
void init_paging()
{
	__asm__ __volatile__ (
		"movl %0, %%eax\t\n"
		"mov %%eax, %%cr3\t\n"
		"mov %%cr0, %%eax\t\n"
		"orl $0x80000000, %%eax\t\n"
		"mov %%eax, %%cr0\t\n"
		"jmp 1f \t\n"			/* flush instruction queue. */
		"1:\t\n"
		:
		:"g" (kernel_map.pdbr)
		:"eax"
		);
}
