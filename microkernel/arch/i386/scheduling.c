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
 * File: scheduling.c
 * The architecture dependent scheduling primitives.
 **/

#include <asm/timer.h>
#include <kernel/scheduling.h>
#include <kernel/kernel_stdio.h>
#include <kernel/vm.h>
#include <asm/pmap.h>
#include <asm/descriptors.h>
#include <asm/8259a.h>
#include <asm/x86.h>
#include <asm/tss.h>

extern thread_t init_thread;

/* Archetecture specfic scheduling information. */
void arch_sched_init()
{
	/* Enable IRQ0 */
	outportb(0xF8, PIC1_DATA);
	outportb(0xFF, PIC2_DATA);
}

/* Clean exited process' pmap and calling thread */
static inline void destroy_all(struct thread *old)
{
	kprintf("Thread Exiting");
	if(old->states & PROCESS_EXITING)
	{
		pmap_destroy(old->proc->map->pmap);
		vm_map_destroy(old->proc->map);
	}
	remove_thread(old);
	destroy_thread(old);
}

/* Switch to a new thread */
void switch_to(struct thread *old, struct thread *new)
{
	struct regs *prev = &old->context.uregs;
	struct regs *next = &new->context.uregs;
	struct TSS *tss;

	/* Update the CR3 register if necessary. */
	if(prev->cr3 != next->cr3)
		write_cr3(next->cr3);

	/* Update stack pointer. */
	stss[new->cpu].esp0 = new->stack;
	stss[new->cpu].cr3 = next->cr3;

	asm volatile(
		"pusha\n\t"
		"movl %%esp, %0\n\t"
		"movl %2, %%esp\n\t"
		"movl $2f, %1\n\t"
		"pushl %3\n\t"
		"ret\n\t"
		"2:\n\t"
		"popa"
		:"=m"(prev->esp), "=m"(prev->eip)
		:"m"(next->esp), "m"(next->eip)
		:"memory"
		);
}

/* Put the thread to uninterruptible sleep */
int syscall_sleep_unint(struct stack_regs *r)
{
	thread_t thread;

	if(!superuser())
		return -EPERM;

	if(r->ebx)
	{
		thread = find_by_tid(r->ebx);
		if(!thread)
			return -ESRCH;
		sleep_uninterruptible(thread);
	}
	else
		sleep_uninterruptible(current);

	return 0;
}

/* Wake the thread from uninterruptible sleep */
int syscall_wake_unint(struct stack_regs *r)
{
	thread_t thread;

	if(!superuser())
		return -EPERM;

	if(r->ebx)
	{
		thread = find_by_tid(r->ebx);
		if(!thread)
			return -ESRCH;

		wakeup(thread);
	}
	else
		wakeup(current);

	return 0;
}
