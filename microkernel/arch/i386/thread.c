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
 * File: thread.c
 *
 *	Architecture Specfic Thread Handling Routines.
 */

#include <asm/context.h>
#include <asm/descriptors.h>
#include <asm/x86.h>

#include <kernel/vm.h>
#include <kernel/scheduling.h>



extern vm_offset_t ending_mem;
extern process_t kernel;

extern void syscall_return();
extern thread_t init_thread;
extern struct pmap kernel_map;
extern vm_offset_t kernel_stack;

/*
 * Architecture Specfic Initialization.
 */
u32 arch_thread_init()
{
	vm_size_t total_threads;
	struct TSS *tss = &stss[0];

	/* maximum threads assuming each thread uses 4 pages Min */
	total_threads = (ending_mem/(PAGE_SIZE * 4));

	tss->esp0 = init_thread->stack;
	tss->ss0 = KERNEL_DATA;

	/* Load The task register with system tss. */
	LTR(0x28);
	return total_threads;
}

/*
 * Create a CPU specfic Context.
 */
void  context_create(thread_t thread)
{
	struct context *ctx;
	struct vm_map *map = thread->proc->map;


	ctx = &thread->context;
	ctx->uregs.esp = thread->stack;
	ctx->uregs.cr3 = (u32)map->pmap->pdbr;

	ctx->irqflag = 0;
	ctx->handler = NULL; 
}

extern void syscall_return_new();

/*
 * Maintain the stack of the child for returning.
 */
struct stack_regs *maintain_stack(thread_t thread, struct stack_regs *regs)
{
	struct stack_regs *chld;

	chld = (struct stack_regs *)(thread->stack - (sizeof(struct stack_regs)));

	if(regs)
		*chld = *regs;
//	else
	{
		chld->user_ss = USER_DATA;
		chld->eflags = EFLAG|EFLAG_IOPL0|EFLAG_ID|EFLAG_INT;
		chld->user_cs = USER_CODE;
		chld->ds = USER_DATA;
		chld->es = USER_DATA;
	}

	chld->eax = 0;
	thread->context.uregs.eip = (u32)syscall_return_new;
	thread->context.uregs.esp = (u32)chld-4 ;
	thread->context.uregs.cr3 = (u32)thread->proc->map->pmap->pdbr;
	*((unsigned int *)((unsigned int)chld - 4)) = (unsigned int)chld;

	return chld;
}
