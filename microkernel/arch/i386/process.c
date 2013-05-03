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
 * File: process.c
 * 	Architecture dependent. Syscall are handled here
 */

#include <errno.h>
#include <string.h>

#include <kernel/vm_map.h>
#include <kernel/kernel_stdio.h>
#include <kernel/scheduling.h>
#include <kernel/message.h>
#include <kernel/timer.h>

#include <asm/context.h>
#include <asm/x86.h>

extern struct pmap kernel_map;

/*
 * Maintain the parameters and some other info
 * in the user stack of module loaded by bootloader
 */
int new_spawn(struct stack_regs *regs)
{
	/* the string and indexptr should be the size of max
	 * size of string in cmdline
	 */
	char string[512];
	char *ptr;
	unsigned int indexptr[64];
	int argc, len , argvlen = 0;
	if(regs->ecx != LOAD_MAGIC)
		return 0;

	ptr = get_argv_string(string, (char *)regs->ebx, &argc, indexptr);
	kprintf("maintaining %s ", regs->ebx);
	put_user_long(argc, (void *)(USER_STACK_TOP));	/* argc */
	put_user_long(USER_STACK_TOP + 1024, (void *)(USER_STACK_TOP + 4));
	while(indexptr[argvlen] != 0)
	{
		put_user_long(indexptr[argvlen], (void *)(USER_STACK_TOP + (4 * argvlen) + 1024));
		argvlen++;
	}
	//put_user_long((USER_STACK_TOP+512), (void *)(USER_STACK_TOP+ 4)); /* argv pointer */

	len = ((unsigned int)ptr - (unsigned int)(&string));
	ker_to_usr((void *)(USER_STACK_TOP + 1024 + ((argc + 1) * 4)), string, len);
	put_user_long(USER_STACK_TOP+ 1024 + len + ((argc + 1) * 4), (void *)(USER_STACK_TOP + 8)); /* envp pointer */
	put_user_long(0, (void *)(USER_STACK_TOP+ 1024 + len + ((argc + 1) * 4))); /* null envp */

	regs->eax = (USER_STACK_TOP);

	return (USER_STACK_TOP);
}

/* Do arch Specfic Cleanup */
void arch_process_destroy()
{
	write_cr3((unsigned long)kernel_map.pdbr);
}

int syscall_fork(struct stack_regs *regs)
{
	process_t child = NULL;
	thread_t thread;
	struct stack_regs *cregs;

//	kprintf("CAlling FORK.");
	child = do_fork(current->proc, DUP_ALL);

	if((int)child < 0)
		return (int)child;

	thread = (thread_t)child->threads.next;
	cregs = maintain_stack(thread, regs);
	enqueue_thread(thread);
	spin_unlock(&child->lock);

	return child->pid;
}

/*
 * Duplicate a process but share the address space.
 * child is guranteed to run first.
 */
int syscall_vfork(struct stack_regs *regs)
{
	process_t child = NULL;
	thread_t thread, cur = current;
	struct stack_regs *cregs;
	pid_t pid;
//	kprintf("CAlling VFORK.");
	child = do_fork(cur->proc, DUP_SHARE);

	if((int)child < 0)
		return (int)child;
	pid = child->pid;
//	kprintf("VFORK pid %d ", pid);
	thread = (thread_t)child->threads.next;
	cregs = maintain_stack(thread, regs);
	enqueue_thread(thread);
	spin_unlock(&child->lock);
	/*
	 * Let the Child Run First. The parent runs after
	 * the child either calls exit or exec.
	 * The behaviour is undefined in multithreaded Process.
	 */
	remove_thread(cur);
	switch_to(cur, thread);


	return pid;
}

int syscall_spawn(struct stack_regs *regs)
{
	process_t child = NULL;
	struct stack_regs *chld;
	struct thread_attr attr, *pattr = NULL;

	kprintf("calling Spawn ");
	if(regs->edi)
	{
		pattr = &attr;
		usr_to_ker((void *)pattr, (void *)regs->edi, sizeof(struct thread_attr));
	}
	child = do_spawn(current->proc, &attr);

	chld = maintain_stack(child->threads.next, regs);

	chld->user_eip = regs->ebx;
	chld->user_esp = regs->ecx;

	return child->pid;
}

/*
 * Exec call
 * ebx = eip
 * ecx = vn_object
 * edx = argv
 * esi = env
 */
int syscall_exec(struct stack_regs *regs)
{
	struct vn_object vn;
	thread_t thr = current;
	process_t proc = thr->proc;
	long *argv, *envp, *newargv;
	unsigned long size = 0, argsize;
	int argc, ret;

	if(!regs->ecx)
		return -EINVAL;

	usr_to_ker((void *)&vn, (void *)regs->ecx, sizeof(struct vn_object));

	regs->user_esp = USER_STACK_TOP;
	argv = (long *)kmem_alloc(PAGE_SIZE);
	envp = (long *)kmem_alloc(PAGE_SIZE);


	ret = exec_kludge(&vn, 0xDFDFFFFF);
	if(ret < 0)
		return ret;

	regs->user_eip = regs->ebx;

	if(regs->edx)
	{
		kprintf("exec: edx %X ", regs->edx);
		newargv = (long *)copy_argv_string(argv,(long *)regs->edx, &argc);
		size = (unsigned long)(newargv) - (unsigned long)(argv);
		ker_to_usr((void *)(USER_STACK_TOP + 1024), (void *)argv, size);
	}
	else
	{
		/* put Null in the vector */
		put_user_long(0, (void *)(USER_STACK_TOP + 1024));
	}

	put_user_long(argc, (void *)(USER_STACK_TOP));	/* argc */
	put_user_long(USER_STACK_TOP + 1024, (void *)(USER_STACK_TOP + 4)); /* argv */


	put_user_long(USER_STACK_TOP + 1024 + size,
				(void *)(USER_STACK_TOP+ 8)); /* envp pointer */
	/*
	 * For main threads, we use stackaddr as end of argv
	 * it is used by pthreads to store structure
	 * The argv and env arg is used after 1024
	 */
	thr->attr.stackaddr = (char *)(USER_STACK_TOP + 12);
	if(regs->esi)
	{
		kprintf("exec: esi %X ", regs->esi);
		newargv = (long *)copy_argv_string(envp, (long *)regs->esi,  &argc);
		argsize = (unsigned long)(newargv) - (unsigned long)(envp);
		ker_to_usr((void *)(USER_STACK_TOP + 1024 + size), (void *)envp, argsize);
	}
	else
	{
		put_user_long(0, (void *)(USER_STACK_TOP + 1024 + size));
	}

	kmem_free((vm_offset_t)argv, PAGE_SIZE);
	kmem_free((vm_offset_t)envp, PAGE_SIZE);

	/* reset registers */
	regs->ebx = 0;
	regs->ecx = 0;
	regs->edx = 0;
	regs->edi = 0;
	regs->esi = 0;
	regs->ebp = 0;

	/* Load the new Page Tables */
	thr->context.uregs.cr3 = (u32)proc->map->pmap->pdbr;
	write_cr3(thr->context.uregs.cr3);

	return (USER_STACK_TOP);
}

int syscall_get_user(struct stack_regs *regs)
{
	process_t process;

	if(!regs->ebx)
		process = current->proc;
	else
	{
		process = find_by_pid(regs->ebx);
		if(!process)
			return -ESRCH;
	}

	regs->ebx = get_euid(process);
	regs->ecx = get_gid(process);
	regs->edx = get_egid(process);

	return 	get_uid(process);
}

/*
 * SYSCALL: Set the group and supplimentary group id.
 * ebx = uid
 * ecx = euid
 * edx = gid
 * esi = egid
 */

int syscall_set_user(struct stack_regs *regs)
{
	process_t process;

	if(!regs->ebx)
		process = current->proc;
	else
	{
		process = find_by_pid(regs->ebx);
		if(!process)
			return -ESRCH;
	}

	if(regs->edx != -1)
		set_euid(process, regs->edx);

	else if(regs->edi != -1)
		set_egid(process, regs->edi);

	if(!superuser())
		return -EPERM;

	if(regs->ecx != -1)
		set_uid(process, regs->ecx);

	else if(regs->esi != -1)
		set_pgrp(process);

	return 0;
}

/* Syscall: Wait till all child exit. */
int syscall_waitpid(struct stack_regs *regs)
{
	return waitpid(regs->ebx, &regs->ecx, regs->edx);
}

/*
 * Change the I/O previlage level of the thread (X86 only ).
 */
int syscall_change_iopl(struct stack_regs *regs)
{
	if(!superuser())
		return -EPERM;
	if((regs->ebx < 0) || (regs->ebx > 3))
		return -EINVAL;

	regs->eflags &= ~EFLAG_IOPL3;

	regs->eflags |= (regs->ebx << IOPL_SHIFT);

	return 0;
}

/* Get the current Working Directory */
int syscall_get_cwd(struct stack_regs *r)
{
	thread_t thr;
	struct fd *fd;

	if(!r->ebx)
		return -EINVAL;

	thr = find_by_tid(r->ebx);
	if(!thr)
		return -EINVAL;
	fd = &thr->proc->fd;

	if(r->ecx)
		ker_to_usr((void *)r->ecx, (void *)&fd->cwd, sizeof(struct inodev));

	if(r->edx)
		ker_to_usr((void *)r->edx, (void *)&fd->cwdstr, strlen(fd->cwdstr)+1);

	return 0;
}

/* Put the current Working Directory */
int syscall_put_cwd(struct stack_regs *r)
{
	thread_t thr;
	struct fd *fd;
	int index;
	char *uptr;

	if(!r->ebx)
		return -EINVAL;

	thr = find_by_tid(r->ebx);
	if(!thr)
		return -EINVAL;
	fd = &thr->proc->fd;

	if(r->ecx)
		usr_to_ker((void *)&fd->cwd, (void *)r->ecx, sizeof(struct inodev));

	if(!r->edx)
		return 0;

	uptr = (char *)r->edx;

	spin_lock(&fd->lock);
	for(index = 0; index < MAX_PATH_LEN; index++)
	{
		fd->cwdstr[index] = get_user_byte(uptr++);
		if(fd->cwdstr[index] == 0)
			break;
	}

	/*
	 * Just making sure it ends with NULL even if
	 *  string is larger.
	 */
	fd->cwdstr[MAX_PATH_LEN] = '\0';
	spin_unlock(&fd->lock);
	return 0;
}

