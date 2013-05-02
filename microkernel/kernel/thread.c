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
 * File: thread.c
 *
 *
 **/

#include <string.h>
#include <kernel/scheduling.h>
#include <kernel/kernel_stdio.h>
#include <kernel/info.h>
#include <kernel/cpu.h>

/* CPU Ticks */
extern u64 ticks;

extern vm_object_t kernel_object;
extern process_t kernel;

u32 max_threads = 0;

typedef struct {
	thread_t thr;
} *thashtable_t;

/* Hash table of threads  */
thashtable_t thash = NULL;
spinlock_t hashlock = SPIN_LOCK_UNLOCKED;

thread_t init_thread = NULL;

extern struct sysinfo sysinfo;

static inline void thash_insert(struct thread *thread)
{
	thashtable_t t;

	spin_lock(&hashlock);
	t = &thash[hash_fn(thread->tid)];
	thread->next = t->thr;
	t->thr = thread;
	spin_unlock(&hashlock);
}

static inline void thash_remove(struct thread *thread)
{
	thashtable_t t;
	thread_t link;

	spin_lock(&hashlock);

	t = &thash[hash_fn(thread->tid)];

	if(t->thr == thread)
	{
		t->thr = thread->next;
	}
	else
	{
		for(link = t->thr; link != NULL; link = link->next)
			if(link->next == thread)
				link->next = thread->next;
	}

	spin_unlock(&hashlock);
}

void  change_tid(thread_t thread, tid_t new)
{
	dealloc_tid(thread->tid);
//	thash_remove(thread);
	thread->tid  = new;
	thash_insert(thread);
}

void sleep_interruptible(thread_t thr)
{
	thr->states = THREAD_INT;
	remove_thread(thr);
	schedule();
}

/* Make the thread uninterruptable sleep */
 void sleep_uninterruptible(thread_t thr)
{
	thr->states = THREAD_UNINT;
	remove_thread(thr);
	schedule();
}

/* Find a thread by its tid. */
thread_t find_by_tid(tid_t tid)
{
	thashtable_t t;
	thread_t link;

	spin_lock(&hashlock);
	t = &thash[hash_fn(tid)];

	if(t->thr->tid == tid)
	{
		link = t->thr;
		goto end;
	}
	else
	{
		for(link=t->thr; link->tid != tid; link=link->next)
		{
			if(link == NULL)
				break;
		}
	}

end:
	spin_unlock(&hashlock);
	return link;
}

thread_t create_thread(struct process *process, struct thread_attr *attr)
{
	thread_t thr, cur=current;

	thr = (thread_t)kmem_alloc(PAGE_SIZE);
	if(!thr)
		return NULL;
//	kprintf("thr %X ", thr);
	//memset(thr, 0, sizeof(struct thread));
	list_init(&thr->thread_list);
	list_init(&thr->runq);
	/* add the thread in the thread list */
	spin_lock(&process->lock);
	list_enqueue(&process->threads, thr, thread_list, thread_t);
	thr->proc = process;
	spin_unlock(&process->lock);

	thr->stack = ((vm_offset_t)thr) + PAGE_SIZE - 1;

	thr->states = THREAD_READY;
	thr->sigpending = 0;
	thr->time_slice = SLICE;
	thr->tid = alloc_tid();
	thr->attr = *attr;
	list_init(&thr->rcvq);
	list_init(&thr->sndq);
	thr->cpu = 0;
	thr->schedule = 0;
	thr->waiting = 0;
	
#ifdef MANRIX_KPREEMPT
	thr->kpreempt = 0;
#endif

	atomic_inc(&sysinfo.total_threads);

	thash_insert(thr);
	/* Inherit if no attr is passed */
	if(attr == NULL /*|| attr->flags & */)
	{
		thr->priority_stat = cur->priority_stat;
		thr->priority_dyn = cur->priority_dyn;
		thr->schedpolicy = cur->schedpolicy;

	}
	else
	{
		thr->priority_stat = attr->prio.sched_priority;
		thr->priority_dyn = attr->prio.sched_priority;
		thr->schedpolicy = attr->policy;
	}

	/* Create hardware specific information. */
	context_create(thr);

	return thr;
}

/* Create an idle Kernel Thread */
thread_t create_idle(int cpu)
{
	thread_t thr;
	process_t process = kernel;
	struct cpu *cpunum;

	thr = (thread_t)kmem_alloc(PAGE_SIZE);
	if(!thr)
	{
		kprintf("IDLE thread creation failed for cpu %d.\n", cpu);
		panic("PANIC: ");
	}
	memset(thr, 0, sizeof(struct thread));
	list_init(&thr->thread_list);
	list_init(&thr->runq);
	/* add the thread in the thread list */
	spin_lock(&process->lock);
	list_enqueue(&process->threads, thr, thread_list, thread_t);
	thr->proc = process;
	spin_unlock(&process->lock);

	thr->stack = ((vm_offset_t)thr) + PAGE_SIZE - 1;

	thr->states = THREAD_READY;
	thr->sigpending = 0;
	thr->time_slice = SLICE;
	thr->tid = 0;
	thr->cpu = cpu;
	thr->schedule = 0;
	thr->waiting = 0;

#ifdef MANRIX_KPREEMPT
	thr->kpreempt = 0;
#endif
	thr->priority_stat = PRIO_SYS_MIN;
	cpunum = get_cpu();
	cpunum->idle = thr;

	atomic_inc(&sysinfo.total_threads);
	thash_insert(thr);

	/* Create hardware specific information. */
	context_create(thr);

	return thr;

}

int destroy_thread(thread_t thread)
{
	dealloc_tid(thread->tid);
	thash_remove(thread);

	/*
	 * If any threading is waiting for it,
	 * wake that thread up.
	 */
	if(thread->waiting)
	{
		wakeup(thread->waiting);
	}

	atomic_dec(&sysinfo.total_process);

	spin_lock(&thread->proc->lock);
	list_del(&thread->proc->threads, thread, thread_list, thread_t);
	spin_lock(&thread->proc->lock);

	kmem_free((vm_offset_t)thread, PAGE_SIZE);

	return 0;
}

void thread_init()
{
	thash = (thashtable_t)kmem_alloc(HASH_SIZE);
	if(!thash)
		panic("thread_init: Not enough Memory to Boot ManRiX\n");

	memset(thash, 0, HASH_SIZE);

	init_thread = current;
	kprintf("thr %X ", init_thread);
//	memset(init_thread, 0, sizeof(struct thread));
	list_init(&init_thread->thread_list);
	list_init(&init_thread->runq);
	 /* add the thread in the thread list */
	list_enqueue(&kernel->threads, init_thread, thread_list, thread_t);

	init_thread->stack = ((vm_offset_t)init_thread)+PAGE_SIZE-1;
	init_thread->proc = kernel;

	/* Create hardware specific information. */
	context_create(init_thread);

	thash_insert(init_thread);
	init_thread->priority_stat = PRIO_SYS_DEF;
	init_thread->priority_dyn = PRIO_SYS_DEF;
	init_thread->schedpolicy = SCHED_OTHER;
	init_thread->time_slice = SLICE;	/* 40 ms where HZ = 100 */
	init_thread->tid = 0;
	init_thread->sigpending = 0;
	init_thread->cpu = 0;
	init_thread->schedule = 0;
	init_thread->waiting = 0;

#ifdef MANRIX_KPREEMPT
	init_thread->kpreempt = 0;
#endif

	list_init(&init_thread->rcvq);
	list_init(&init_thread->sndq);

	max_threads = arch_thread_init();
}

/* Destroy all the threads belonging to the process, except current. */
void exit_threads(list_t process)
{
	thread_t cur = current, thr;
	int end = 0;

	do
	{
		if(list_isempty(process))
			break;
		list_dequeue(process, thr, thread_list, thread_t);
		if(thr == cur)
		{
			list_enqueue(process, cur, thread_list, thread_t);
			end++;
			continue;
		}
		destroy_thread(thr);
	}while(end < 2);
}

/* Wait for thread to terminate */
int thread_wait(tid_t tid, int *stat)
{
	thread_t thr, cur =  current;

	thr = find_by_tid(tid);
	if(thr == NULL)
		return -ESRCH;

	thr->waiting = cur;
	sleep_uninterruptible(thr);

	return 0;
}

void exec_thread(thread_t thr)
{
	if(thr->waiting)
		wakeup(thr->waiting);

	thr->waiting = NULL;

	list_init(&thr->rcvq);
	list_init(&thr->sndq);
}

/* SYSCALL:Get the tid of the current thread */
int syscall_gettid()
{
	return (current->tid);
}

/* Get the base of user stack */
vm_offset_t syscall_get_ustack()
{
	return (vm_offset_t)current->attr.stackaddr;
}
