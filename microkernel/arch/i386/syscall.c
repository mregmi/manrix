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
 * File: syscall.c
 *
 * Architecture dependent. Syscall are handled here
 **/

#include <errno.h>
#include <string.h>

#include <kernel/vm_map.h>
#include <kernel/kernel_stdio.h>
#include <kernel/scheduling.h>
#include <kernel/message.h>
#include <kernel/timer.h>

#include <asm/context.h>

int syscall_create_thread(struct stack_regs *regs)
{
	process_t process = current->proc;
	thread_t thr;
	struct thread_attr attr;
	struct stack_regs *chld;
//	int ret;

	if(!regs->ebx)
		return -1;

	usr_to_ker((void *)&attr, (void *)regs->ebx, sizeof(struct thread_attr));


//	kprintf("CAlling Thread_create. stack %x, size %x, policy %d",
//		 attr.stackaddr, attr.stack_size, attr.policy);
	thr = create_thread(process, &attr);
	if(!thr)
		return -1;
	chld = maintain_stack(thr, NULL);
	chld->user_eip = regs->ecx;
	chld->user_esp = (unsigned int)attr.stackaddr + attr.stack_size - sizeof(void *);

	enqueue_thread(thr);
	return thr->tid;
}

/* Destroy a thread */
int syscall_destroy_thread(struct stack_regs *regs)
{
	thread_t thr = current;
	kprintf("Destroying thread");
	remove_thread(thr);
	thr->states = THREAD_EXITING;
	thr->exitstat = regs->ebx;
	schedule();
	return 0;
}

/* join a thread */
int syscall_thread_join(struct stack_regs *regs)
{
	return thread_wait(regs->ebx, &regs->ecx);
}

/* The message passing primitive.
 * This system call sends the message to a particular thread. The thread
 * cannot receive the message unless it does a receive call.
 * So, the calling thread must block.
 * We eliminate the overhead of memory copying by directly mapping the message
 * structure to the saved registers in the stack.
 */
int syscall_send_message(struct stack_regs *regs)
{
	struct message *msg;
	char data[MAX_MSG_LONG];	/* Data on kernel stack for MSG_LONG */
//	thread_t thr = current;
	void *rcvptr;

	msg = (message_t)(regs);

	rcvptr = msg->rcv_ptr;
	if(msg->flags & MSG_LONG_SEND)
	{
		if(msg->snd_bufsize > MAX_MSG_LONG)
			return -1;
		usr_to_ker((void *)&data, (void *)msg->snd_ptr,  msg->snd_bufsize);
		msg->snd_ptr = &data;
	}

	send_message(msg->tid, msg);

	if(msg->flags & MSG_LONG_RECEIVE)
	{
		if(msg->rcv_bufsize > MAX_MSG_LONG)
			return -1;
		ker_to_usr((void *)rcvptr, (void *)msg->rcv_ptr, msg->rcv_bufsize);
//		kprintf("copying from %x to %x bytes %d",msg->rcv_ptr, rcvptr, msg->rcv_bufsize);
	}

	return (msg->flags);
}

int syscall_receive_message(struct stack_regs *regs)
{
	struct message *msg;
	unsigned long rcv_pt;
	int ret;

	msg = (message_t)(regs);
 	rcv_pt = (unsigned long)msg->snd_ptr;

//	if(current->tid == 2)
//		print_regs(regs);
//	kprintf("msg->rcv %x ", rcv_pt);
//	print_regs(regs);

	ret = receive_message(msg->tid, msg);
	if(ret < 0)
	{
		kprintf("msg->w1 failed ");
		return ret;
	}

	if(msg->flags & MSG_LONG_SEND)
	{
		ker_to_usr((void *)rcv_pt, msg->snd_ptr, msg->snd_bufsize);
		msg->snd_ptr = (void *)rcv_pt;
	}
//	if(current->tid == 2)
//		print_regs(regs);
	
	return  (msg->flags);
}

int syscall_reply_message(struct stack_regs *regs)
{
	struct message *msg;
	char data[MAX_MSG_LONG];	/* Data on kernel stack for MSG_LONG */

	msg = (message_t)(regs);

	if(msg->flags & MSG_LONG_RECEIVE)
	{
		if(msg->rcv_bufsize > MAX_MSG_LONG)
			return -1;
//		kprintf("pp %x ", msg->rcv_ptr);
		usr_to_ker((void *)&data, (void *)msg->rcv_ptr,  msg->rcv_bufsize);
		msg->rcv_ptr = &data;
//		kprintf("replying from %x  bytes %d",msg->rcv_ptr, msg->rcv_bufsize);
	}

	/* The message replyed by the tid is copied in the same buffer. */

	reply_message(msg->tid, msg);
//	kprintf("tid %d reply", msg->tid);
	return 0;
}


/* Get the scheduling parameters. */
int syscall_sched_get(struct stack_regs *regs)
{
	struct sched_param parm, *pparm = NULL;
	int pol;

	if(regs->ecx)
		pparm = &parm;

	pol = sched_get(regs->ebx, pparm, regs->edx);

	if(regs->ecx)
		ker_to_usr((void *)regs->ecx, (void *)pparm, sizeof(struct sched_param));

	return pol;
}

/*
 * SYSCALL: set the scheduling parameters
 * When policy is passed as 0, ony priority is set.
 * When priority is passed as 0, only policy is set.
 */
int syscall_sched_set(struct stack_regs *regs)
{
	struct sched_param parm, *pparm = NULL;

	if(regs->edx)
	{
		usr_to_ker((void *)&parm, (void *)regs->edx, sizeof(struct sched_param));
		pparm = &parm;
	}
	return sched_set(regs->ebx, regs->ecx, pparm, regs->edi);
}


/*
 * Syscall for allocating new FD and put
 * the open table to the kernel.
 */
int syscall_fd_alloc(struct stack_regs *regs)
{
	thread_t thread;
	struct fd *fd;
	struct file *file;
	int desc = 0, flag = regs->ecx;
	vm_offset_t *ptr;

	/* Previlaged programs only */
	if(!superuser())
		return -EPERM;

	if(!regs->ebx)
		return -EINVAL;

	thread = find_by_tid(regs->ebx);
	if(!thread)
		return -EINVAL;
	fd = &thread->proc->fd;

	if(flag < 0)
		desc = alloc_fdesc(fd);

	if(!regs->edx)
		return desc;

	if(desc < MAX_DEF_FD)
		file = (struct file *)fd->fileptr[desc];
	else
	{
		ptr = (vm_offset_t *)fd->fileptr[MAX_DEF_FD];
		file = (struct file *)ptr[desc - MAX_DEF_FD];
	}

	usr_to_ker((void *)file, (void *)regs->edx, sizeof(struct file));
//	kprintf("err desc %d file off %d vnode %d ", desc, file->f_offset, file->vnode.inode);

	return desc;
}

/*
 * Syscall for allocating new FD
 * and get the open file table.
 */
int syscall_fd_free(struct stack_regs *regs)
{
	thread_t thread;
	struct fd *fd;
	struct file *file;
	int desc = regs->ecx;
	vm_offset_t *ptr;

	/* Previlaged programs only */
	if(!superuser())
		return -EPERM;

	if(!regs->ebx)
		return -EINVAL;

	thread = find_by_tid(regs->ebx);
	if(!thread)
		return -EINVAL;
	fd = &thread->proc->fd;

	if(desc < 0)
		free_fdesc(fd, regs->ecx);

	if(!regs->edx)
		return desc;

	if(desc < MAX_DEF_FD)
		file = (struct file *)fd->fileptr[desc];
	else
	{
		ptr = (vm_offset_t *)fd->fileptr[MAX_DEF_FD];
		file = (struct file *)ptr[desc - MAX_DEF_FD];
	}

	ker_to_usr((void *)regs->edx, (void *)file, sizeof(struct file));
//	kprintf("ll desc %d file off %d vnode %d ", desc, file->f_offset, file->vnode.inode);

	return desc;
}
