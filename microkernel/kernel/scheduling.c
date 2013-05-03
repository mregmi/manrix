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
 *		The Scheduling Functions.
 **/
#include <kernel/scheduling.h>
#include <kernel/types.h>
#include <kernel/kernel_stdio.h>
#include <kernel/sync.h>
#include <kernel/cpu.h>
#include <asm/asm.h>	/* for ffs() find first set bit */


extern thread_t init_thread;

/* CPU Ticks */
extern u64 ticks;

/* Total no of processors */
int N_PROCS	= 1;

/*
 * Run queue for scheduling.
 * The is a runqueue for each processor.
 */
struct rqueue runqueue[MAX_CPU];

/* Hints pointing to least loaded cpu */
struct load_hint hint = { 0, SPIN_LOCK_UNLOCKED };

/* Initialize the scheduling. */
void sched_init()
{
	int index, rqindex;
	list_t list;
	struct rqueue *rq;

	/* Architecture specfic setup. */
	arch_sched_init();
	memset(&runqueue, 0, sizeof(runqueue));
	/* Initialize the run queues. */
	for(rqindex = 0; rqindex < MAX_CPU; rqindex++)
	{
		rq = &runqueue[rqindex];
		rq->bitmap = 0;

		for(index = 0; index < MAXQ; index++)
		{
			list = &rq->run[index];
			list_init(list);
		}
	}
//	kprintf("hint = %d bmap %X", hint.cpu_hint, runqueue[0].bitmap);
//	enqueue_thread(init_thread);
}

/*
 * do round robin
 */
static inline void do_schedrr()
{
	struct rqueue *currq;
	int quenum;
	list_t list;
	thread_t thread;

	currq = get_cur_runqueue();
	quenum = frs(currq->bitmap);
	if(quenum < 0)
		return;

	list = &currq->run[quenum];
	if(list_isempty(list))
		return;

	thread = dequeue_thread(quenum, current->cpu);
	if(!thread)
		return;
	enqueue_thread(thread);
}

/*
 * This function is called by timer on each timer interrupt.
 * It does the timeslice and dynamic priority management.
 */
void sched_timer()
{
	thread_t thr = current;

	if(thr->schedpolicy == SCHED_RR)
	{
//		do_schedrr();
		return;
	}

	if(thr->schedpolicy != SCHED_OTHER)
		return;

	if(thr->priority_stat >= PRIO_SYS_MIN)
	{
//		do_schedrr();
		return;
	}

	/*
	 * Its time slice has expired so
	 * recalculate its timeslice. Also recalculate its dynamic
	 * priority and enqueue to updated runqueue.
	 */
	if(!(--thr->time_slice))
	{
		RECALC_SLICE(thr);

		if((--thr->bound) < PRIO_OTHER_MIN)
			thr->bound = PRIO_OTHER_MIN;

		remove_thread(thr);
		thr->priority_dyn = thr->priority_stat + thr->bound;
		enqueue_thread(thr);
	}
}

/*
 * the main scheduler function.
 */
void schedule()
{
	thread_t new;
	thread_t thr = current;
	list_t list;
	int quenum;
	struct cpu *cpu;
	struct rqueue *currq;

	/* If the preemption is disabled, do not switch threads */
	if(on_kpreempt())
		return;

	kpreempt_disable();	/* Disable kernel preemption */

	currq = get_cur_runqueue();
	quenum = frs(currq->bitmap);
again:
	if(quenum < 0)
	{
		cpu = get_cpu();
		new = get_idle_thread(cpu);
	}
	else
	{
		list = &currq->run[quenum];
		if(list_isempty(list))
		{
			quenum = -1;
			goto again;
		}
		last_entry(list, new);
	}

	/* Architecture specfic thread switching. */
	if(new != thr)
		switch_to(thr, new);
//	else
//		kprintf("BOTH SAME %d %d ",thr->tid, new->tid);

	current->schedule = 0;
	kpreempt_enable();	/* re-enable Kernel preemption */
}

#ifdef MANRIX_SMP

/* Update the CPU load hint */
void update_cpu_hint()
{
	int i, nthreads;

	spin_lock(&hint.lock);
	nthreads = atomic_get(&runqueue[0].nthreads);
	hint.cpu_hint = 0;
	for(i = 1; i < N_PROCS; i++)
	{
		if(nthreads > atomic_get(&runqueue[i].nthreads))
		{
			nthreads = atomic_get(&runqueue[i].nthreads);
			hint.cpu_hint = i;
		}
	}
	spin_unlock(&hint.lock);
}

#endif	/* MANRIX_SMP */

/* Get the scheduling parameters */
int sched_get(int id, struct sched_param *parm, int flag)
{
	thread_t thr;
	process_t p;

	if(!superuser())
		return -EPERM;

	if(id == 0)
		thr = current;
	else
	{
		if(flag == ID_THREAD)
			thr = find_by_tid(id);
		else
		{
			p = find_by_pid(id);
			thr = get_main_thread(p);
		}
	}

	parm->sched_priority = thr->priority_stat;

	return thr->schedpolicy;
}

int syscall_sched_yield()
{
	thread_t thr = current;

	remove_thread(thr);
	enqueue_thread(thr);

	sched_set_pending(thr);

	return 0;
}

int sched_set(int id, int pol, struct sched_param *parm, int flag)
{
	thread_t thr;
	process_t p;

	if ((pol > SCHED_OTHER)
	|| (parm->sched_priority > MIN_PRIO || parm->sched_priority < MAX_PRIO))
		return -1;

	if(id == 0)
		thr = current;
	else
	{
		if(flag == ID_THREAD)
			thr = find_by_tid(id);
		else
		{
			p = find_by_pid(id);
			thr = get_main_thread(p);
		}
	}

	if(!superuser())
		return -EPERM;

	remove_thread(thr);

	if(pol)
		thr->schedpolicy = pol;

	if(parm)
	{
		thr->priority_dyn = thr->priority_stat = parm->sched_priority;
		thr->bound = 0;
	}

	enqueue_thread(thr);

	return thr->schedpolicy;
}

