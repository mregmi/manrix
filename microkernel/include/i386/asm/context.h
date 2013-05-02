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
 * File: context.h
 *	Architecture Specfic Thread Context.
 */

#ifndef __I386_CONTEXT_H
#define __I386_CONTEXT_H

#include <string.h>

#include <asm/tss.h>
#include <asm/descriptors.h>
#include <kernel/vm_param.h>

/* irqflag values. */
#define IRQ_REQUEST		1
#define ON_IRQHAND		2

#define LOAD_MAGIC		0xBAADBAAD

/*
 * For context Switching: are other registers required?
 * The user regs are already saved during interrupt/syscall.
 */
struct regs{
	u32 eip;
	u32 esp;

/*	u32 esi;
	u32 edi;
	u32 ebp;
	u32 esp;
	u32 ebx;
	u32 edx;
	u32 ecx;
	u32 eax;*/

	u32 cr3;
};

/*
 * cpu context of the thread.
 */
struct context {
	struct regs uregs;

	/* For IRQ handler threads */
	int irqflag;
	void (* handler)(void);

};

/*
 * Layout of the registers in kernel stack after system call or interrupt
 */
struct stack_regs {
	u32 edi;
	u32 esi;
	u32 ebp;
	u32 esp;
	u32 ebx;
	u32 edx;
	u32 ecx;
	u32 eax;
	u16 es, __es;
	u16 ds, __ds;

	u32 error_code;
	u32 user_eip;
	u16 user_cs, __cs;
	u32 eflags;
	u32 user_esp;
	u16 user_ss, __ss;
	};

/*
 * Copy the argument String from user to
 * exed process
 */
static inline char *copy_argv_string(long *argv, long *uargv, int *argc)
{
	unsigned long ptr;
	int narg = 0, index = 0;
	char *str, chr, *ustr;
	long *base = argv;

	do{
		ptr = get_user_long(uargv++);
		*base++ = ptr;
		narg++;
	}while(ptr != 0);
	*base = 0;

	str = (char *)(base + 1);
	while(argv[index] != 0)
	{
		ustr = (char *)argv[index];
		argv[index] = (long)str;
		do{
			chr = get_user_byte(ustr++);
			*str++ = chr;
		}while(chr != '\0');
		index++;
	};

	return str;
}

/*
 * Copy the argument String for process loaded
 * by the bootloader.
 */
static inline char *get_argv_string(char *argv, char *uargv, int *argc, unsigned int *ptr)
{
	char data;
	int narg = 0, arg;
	char *base = uargv, *top = uargv;

	/* First find the argc */
	while(1)
	{
		data = *uargv++;
		if(data == '\0')
		{
			narg++;
			break;
		}
		if(data == ' ')
		{
			while(*uargv == ' ') uargv++;

			narg++;
			continue;
		}
	}
	*argc = narg;
	arg = narg+1;

	narg = 0;
	ptr[narg] = (unsigned int)(((unsigned int)top - (unsigned int)base) +
			USER_STACK_TOP + 1024 + (arg*4));
	while(1)
	{
		data = *top++;
		if(data == '\0')
		{
			narg++;
			break;
		}
		if(data == ' ')
		{
			while(*top == ' ') top++;

			narg++;
			ptr[narg] = ((unsigned int)top - (unsigned int)base) +
					USER_STACK_TOP + 1024 + (arg*4);
			*argv++ = '\0';
			continue;
		}
		*argv++ = data;
	}
	*argv = '\0';
	ptr[narg] = 0;

	return argv;
}

/*
 * The famous way of getting current thread's
 * TCB.
 */
static inline struct thread *get_current()
{
	register struct thread *thr;
	asm volatile(
			"andl %%esp, %0\n\t"
			:"=r"(thr)
			:"0"(~(PAGE_SIZE-1))
			:"esp"
			);

	return thr;
}

#define current get_current()

/* thread initialize for architecture dependent process */
extern u32 arch_thread_init();
extern void  context_create(struct thread *thread);
/* Maintain the stack of the child for returning. */
extern struct stack_regs *maintain_stack(struct thread *thread, struct stack_regs *regs);

extern void arch_sched_init();
extern void switch_to(struct thread *, struct thread *);
extern void print_regs(struct stack_regs *r);

#endif  /* __I386_CONTEXT_H */

