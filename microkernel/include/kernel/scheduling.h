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
 * File: scheduling.h
 *		The Scheduling Macros and Structures.
 **/

#ifndef __KERNEL_SCHEDULING_H
#define __KERNEL_SCHEDULING_H

#include <kernel/config.h>
#include <kernel/error.h>
#include <kernel/types.h>
#include <kernel/list.h>
#include <asm/asm.h>

/* Total number of priorities. */
#define NPRIORS				128

/* Total queues in runqueue */
#define MAXQ				(NPRIORS/4)

/* Max wait queues */
#define MAX_WQ				12

/* Fixed 40ms time slice */
#define SLICE				(4 * (HZ/100))

/* Scheduling Policies supported by system */
#define SCHED_FIFO			1
#define SCHED_RR			2
#define SCHED_OTHER			4

/* Different class of priorities. */
#define PRIO_RT_MAX			127		/* For Real-Time threads. */
#define PRIO_RT_DEF			107
#define PRIO_RT_MIN			88

#define PRIO_SYS_MAX			87
#define PRIO_SYS_DEF			66		/* For System-programs. */
#define PRIO_SYS_MIN			46

#define PRIO_OTHER_MAX			45
#define PRIO_OTHER_DEF			24		/* For Others. */
#define PRIO_OTHER_MIN			4

#define PRIO_IDLE			0		/* for idle threads */

#define MIN_PRIO			PRIO_OTHER_MIN
#define MAX_PRIO			PRIO_RT_MAX

#define PRIOQ(prio)			((prio)/4)

#define WAIT_SEND			1
#define WAIT_RECEIVE			2
#define WAIT_REPLY			4
#define WAIT_TIMER			8
#define WAIT_WAIT			16

#define ID_THREAD			1
#define ID_PROCESS			2

#define HASH_SIZE			4096	/* Only one page of hash table is used */
#define hash_fn(x)			((((x)>>8)^(x))&((HASH_SIZE/sizeof(void *)) - 1))

#define superuser()			((current->proc->euid == 0)	? 1 : 0)

#define RECALC_SLICE(thr)	\
{				\
	thr->time_slice = SLICE;\
}

#define MANRIX_BUG()	\
{			\
	panic("Kernel Panic");	\
}


#include <kernel/process.h>
#include <kernel/thread.h>
#include <kernel/kpreempt.h>

#define sched_set_pending(thr)		(thr->schedule = 1)

/* runqueue. */
struct rqueue {
	volatile unsigned int bitmap;
	struct linked_list run[MAXQ];
	atomic_t nthreads;
};

struct wait_queue{
	struct linked_list wq;
};

struct load_hint{
	int cpu_hint /*, cpu_hint2*/;	/* Hint pointing to least loaded cpu */
	spinlock_t lock;
};

extern int N_PROCS;		/* total no of processors */
extern struct rqueue runqueue[MAX_CPU];
extern struct load_hint	hint;
extern thread_t init_thread;
/*
 * Enqueue the thread to the least loaded run queue.
 * TODO: check if it is cache hot.
 */
static inline void enqueue_thread(thread_t thread)
{
	int index;
	list_t rqlist;

	kpreempt_disable();

	index = PRIOQ(thread->priority_dyn);
	rqlist = &runqueue[hint.cpu_hint].run[index];

	atomic_inc(&runqueue[hint.cpu_hint].nthreads);
	list_enqueue(rqlist, thread, runq, thread_t);
	set_bit(&runqueue[hint.cpu_hint].bitmap, index);
	thread->cpu = hint.cpu_hint;
	kpreempt_enable();
}

/* dequeue the thread from the run queue */
static inline thread_t dequeue_thread(int index, int cpu)
{
	thread_t thread;
	list_t l;

	kpreempt_disable();

	l = &runqueue[cpu].run[index];
	list_dequeue(l, thread, runq, thread_t);
	if(list_isempty(l))
		clear_bit(&runqueue[cpu].bitmap, index);

	kpreempt_enable();

	return thread;
}

/*
 * Remove the thread from the Run Queue.
 */
static inline void remove_thread(thread_t thread)
{
	int index;
	list_t list;

	kpreempt_disable();
	index = PRIOQ(thread->priority_dyn);
	list = &runqueue[thread->cpu].run[index];
	list_del(list, thread, runq, thread_t);

	if(list_isempty(list))
		clear_bit(&runqueue[thread->cpu].bitmap, index);

	atomic_dec(&runqueue[thread->cpu].nthreads);
	kpreempt_enable();
}

/* wake the thread up */
static inline void wakeup(thread_t thr)
{
	thr->states = THREAD_READY;
	thr->priority_dyn = thr->priority_stat;
	enqueue_thread(thr);
}

/* Initialize the scheduling. */
extern void sched_init();
extern void sched_timer();
extern void schedule();
extern void wakeup(thread_t thr);
extern int sched_get(int , struct sched_param *, int );
extern int sched_set(int , int , struct sched_param *, int );


extern void id_init();
extern int alloc_pid();
extern int alloc_tid();
extern void dealloc_pid(pid_t );
extern void dealloc_tid(tid_t );
extern int alloc_timerid();
extern void dealloc_timerid(int );

#endif /* __KERNEL_SCHEDULING_H */
