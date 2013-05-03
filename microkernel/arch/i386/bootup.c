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
 * File: bootup.c
 *
 * This is a helper routine which will configure the modules loaded by
 * the boot loader.
 */
#include <string.h>

#include <asm/multiboot.h>
#include <asm/descriptors.h>

#include <kernel/kernel_stdio.h>
#include <kernel/elf.h>
#include <kernel/scheduling.h>
#include <kernel/vm.h>

/*
 * Check for the ELF Magic string.
 */
#define checkmagic(e)	\
	((e->e_ident[EI_MAG0] == 0x7F) && \
	(e->e_ident[EI_MAG1] == 'E') &&	\
	(e->e_ident[EI_MAG2] == 'L') &&	\
	(e->e_ident[EI_MAG3] == 'F'))

extern struct multiboot_info *minfo;
extern process_t kernel;

struct servers{
	int tid;
	const char *name;
} serv[] = {
	{ 1, "/sys/conmngr"},
	{ 2, "/sys/fsiomngr"},
	{ 3, "/sys/atamngr"},
	{ 4, "/sys/misciomngr"},
	{ 5, "/sys/busmngr"},
	{ 6, "/bin/init"},
	{ 0, NULL}
};

static inline int get_tid_from_name(const char *name)
{
	char svrname[255];
	int index;

	for(index = 0; ; index++)
	{
		if((name[index] == ' ') || (name[index] == '\0'))
			break;
		svrname[index] = name[index];
	}
	svrname[index] = '\0';	/* Terminate with NULL */

	for(index = 0; ; index++)
	{
		if(serv[index].name == NULL)
			return 0;
		if(strcmp(serv[index].name, svrname) == 0)
		{
//			kprintf("%s tid is %d\n", svrname, serv[index].tid);
			return serv[index].tid;
		}
	}

	return 0;
}

/*
 * Create the pages for the memory Occupied by the Modules.
 * The vmpage.c does not create vm_page structures for this Memory.
 */
static inline int create_pages(vm_offset_t offset, vm_offset_t vm, vm_map_entry_t entry, size_t size)
{
	int ret;
	vm_offset_t pa = offset, va = vm;

	while(size != 0)
	{
		ret = create_vm_page(pa, va, entry);
		pa += PAGE_SIZE;
		va += PAGE_SIZE;

		size--;
	}

	return 0;
}

/*
 * Create the Memory Context for the Process.
 * Create the vm_map_entry for Stack and the Image.
 */
static void create_mem_context(process_t p, ELF32_Ehdr *hdr, module_t mod)
{
	Elf32_Phdr *phdr;
	Elf32_Shdr *shdr;
	int i, psize;
	vm_offset_t addr, stack = 0xDFDFFFFF, alg;
	vm_map_entry_t entry;

	psize = mod->mod_end - mod->mod_start;

	for(i = 0; i < hdr->e_phnum; i++)
	{
		phdr = (Elf32_Phdr *) ((u32)hdr + hdr->e_phoff + (hdr->e_phentsize * i));

		addr = (u32)hdr + phdr->p_offset;

		pmap_enter_range(p->map->pmap, addr, phdr->p_vaddr, phdr->p_memsz, PAGE_USER|PAGE_RW);
	}
	shdr = (Elf32_Shdr *) ((u32)hdr + hdr->e_shoff);
	pmap_enter(kernel->map->pmap, (u32)shdr, (u32)shdr + KERNEL_BASE, PAGE_USER|PAGE_RW);
	
	/* Zero out BSS */
/*	for(i = 0; i < hdr->e_shnum; i++)
	{
		shdr = (Elf32_Shdr *) ((u32)hdr + hdr->e_shoff + (hdr->e_shentsize * i));
		addr = (u32)hdr + shdr->sh_offset;

		if((shdr->sh_type == SHT_NOBITS) && (shdr->sh_flags & (SHF_ALLOC | SHF_WRITE)))
		{
			kprintf("Hoorajh \n");
			memset((void *)addr, 0, shdr->sh_size);
			break;
		}
	}
*/
	vm_map_enter(p->map, &hdr->e_entry, psize, NULL, VM_MAP_ENTRY_OBJECT_FIXED);
	create_pages((vm_offset_t)hdr, hdr->e_entry, (vm_map_entry_t)p->map->vmentry,
		       			align_page(psize)/PAGE_SIZE);

	/* map for stack */
	vm_map_enter(p->map, &stack, USER_STACK_ENTRY_SIZE,
				NULL, VM_MAP_ENTRY_OBJECT_FIXED|VM_MAP_ENTRY_ZERO_FILL);

	kprintf("entry %x size %x end %x", hdr->e_entry, psize, hdr->e_entry+psize);
}

/*
 * Create a Process context for the Loaded Module.
 * The Process are simply spawned rather than Forking+execing.
 */
int boot_spawn(tid_t tid, vm_offset_t entry, vm_offset_t stack, module_t m)
{
	process_t p;
	thread_t thr;
	struct stack_regs *chld;
	struct thread_attr attr;
	unsigned long *ptr;
	tid_t ntid;

	attr.prio.sched_priority = PRIO_SYS_DEF;
	attr.policy = SCHED_OTHER;

	p = do_spawn(current->proc, &attr);

	thr = (thread_t)p->threads.next;
	ntid = get_tid_from_name((char *)m->string);
	if(tid == 0)
	{
		kprintf("Unknown module %s \n", m->string);
		ntid = tid;
	}
	change_tid(thr, ntid);
	change_pid(p, ntid);

	chld = maintain_stack(thr, NULL);
	chld->user_eip = entry;
	chld->user_esp = stack;
	chld->ebx = m->string;
	chld->ecx = LOAD_MAGIC;
	ptr = (unsigned long *)((unsigned long)chld - 4);
	*ptr = (unsigned long)chld;
	thr->attr.stackaddr = (void *)(USER_STACK_TOP + 12);

	/*
	 * The Process in User mode needs this Value.
	 * see inifini.c in the ManRiX library.
	 */
	chld->eax = USER_STACK_TOP;

	return p->pid;
}

/*
 * Initialize the Modules Loaded by
 * the Boot Loader.
 */
void modules_init()
{
	module_t m;
	ELF32_Ehdr *ehdr;
	int i;
	vm_offset_t stack = 0xDFEFFFFF;

	pid_t pid;
	thread_t thr;
	process_t p;


	for(i=0;i < minfo->mods_count;i++)
	{
		m = (module_t)(minfo->mods_addr + (i * sizeof(struct module)));
		ehdr = (ELF32_Ehdr *)m->mod_start;
		pmap_enter(kernel->map->pmap, m->mod_start, m->mod_start+KERNEL_BASE, PAGE_USER|PAGE_RW);

		if(!checkmagic(ehdr))
		{
			kprintf("Cannot load '%s'. Not in Valid Elf Format.\n", m->string);
			continue;
		}

		if(ehdr->e_type != ET_EXEC)
		{
			kprintf("'%s' is not an executable Elf File.\n", m->string);
			continue;
		}

		if(ehdr->e_machine != EM_386)
		{
			kprintf("'%s' is not an x86 executable Elf File.\n", m->string);
			continue;
		}

		pid = boot_spawn(i+100, ehdr->e_entry, stack, m);

		p = find_by_pid(pid);
		if(!p)
			panic("BUG");
		create_mem_context(p, ehdr, m);

		thr = (thread_t)p->threads.next;
//		kprintf("\nModule %s loaded, tid %d pid %d", m->string, thr->tid, pid);
	}

}
