/**
 * ManRiX Microkernel
 * Copyright (C) 2004, 2005
 * 	Manish Regmi (regmi dot manish at gmail dot com) and
 * 	Rajesh R.C   (rajesh dot rc at gmail dot com)
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
 * File: timer.c
 *
 * Timer functions.
 **/

#include <string.h>

#include <kernel/timer.h>
#include <kernel/scheduling.h>
#include <kernel/error.h>
#include <kernel/vm.h>
#include <kernel/signal.h>
#include <kernel/kernel_stdio.h>

/* Current time */
struct timespec ctime = {0};

//extern struct rqueue runqueue;

/* The resolution of timers */
unsigned int timer_res[MAXTIMERS] =	{
					1000000000/HZ, /* CLOCK_REALTIME */
					0 /* Others not implemented yet */
					};

/* List of timer. */
struct timer tmlist;

/* Timer cache */
kmem_cache_t tm_cache = NULL;

/* Current tick value. Updated by timer interrupt. */
volatile u64 ticks = 0;

/* Hash table of all the timers. */
timer_t tmhash = NULL;

/* OS initialization */
void time_init()
{
	timer_init();

	ctime.tv_sec = get_time();
	ctime.tv_nsec = 0;

	list_init(&tmlist.tlist);
	tm_cache = kmem_cache_create("tm_cache",sizeof(struct timer), 16, NULL, NULL);
	if(!tm_cache)
		panic("time_init: Not Enough Memory to boot system\n");
	tmhash = (timer_t)kmem_alloc(PAGE_SIZE);
	if(!tmhash)
		panic("time_init: Not Enough Memory to boot system\n");

	memset(tmhash, 0, PAGE_SIZE);
}

/* Insert the timer in the global hash table. */
static inline void tmhash_insert(timer_t timer)
{
	timer_t tm;

	tm = &tmhash[hash_fn(timer->timer_id)];
	timer->next = tm;
	tm = timer;
}

/* Remove the timer from the global hash table. */
static inline void tmhash_remove(timer_t timer)
{
	timer_t tm, link;

	tm = &tmhash[hash_fn(timer->timer_id)];

	if(tm == timer)
	{
		tm = timer->next;
	}
	else
	{
		for(link=tm; (link->next!=timer) || (link->next != NULL); link = link->next)
		;
		link->next = timer->next;
	}
}

/* Find a timer by its id. */
timer_t timer_by_id(int timer_id)
{
	timer_t tm, link;

	tm = &tmhash[hash_fn(timer_id)];

	/* optimize for average case. */
	if(tm->timer_id == timer_id)
		return tm;
	else
	{
		for(link=tm->next; link != NULL; link=link->next)
			if(link->timer_id == timer_id)
				return link;
	}
	return NULL;
}


/*
 * Insert the timer item to the global timer list.
 */
void insert_timer(timer_t head, timer_t item)
{
	timer_t tm;

	if(list_isempty(&head->tlist))
		list_enqueue(&head->tlist, item, tlist, timer_t);

	for(tm = (timer_t)head->NEXT_TIMER; tm->timeout > item->timeout; tm = (timer_t)tm->NEXT_TIMER)
		;
	list_add_next((timer_t)tm->PREV_TIMER, tlist, timer_t, item, tlist, timer_t);
}

static inline void check_timeouts()
{
	timer_t lentry;
	thread_t thr = NULL;

again:
	if(list_isempty(&tmlist.tlist))
		return;
	last_entry(&tmlist.tlist, lentry);
	if(lentry->timeout >= ticks)
	{
		list_dequeue(&tmlist.tlist, lentry, tlist, timer_t);
		if(lentry->flag & TM_SLEEP)
		{
			thr = lentry->owner;
			thr->states = THREAD_RUNNING;
			enqueue_thread(thr);
			delete_timer(lentry->timer_id);
		}
		else if(lentry->flag & TM_TIMER)
		{
			switch(lentry->sigv.sigev_notify)
			{
				case SIGEV_NONE:
					break;
				case SIGEV_SIGNAL:
					send_signal_any(lentry->owner->proc, lentry->sigv.sigev_signo, NULL);
					break;
				case SIGEV_THREAD:
					//FIXME:
					break;
			}
			if(lentry->flag & TM_ONESHOT)
			{
				delete_timer(lentry->timer_id);
				thr->proc->alarm = NULL;
			}
			/* rearm the timer */
			if(!lentry->nextarm)
				goto again;

			lentry->timeout += lentry->nextarm;
			insert_timer(&tmlist, lentry);

		}
		goto again;
	}
}



void timer_int()
{
	ticks++;
//	kprintf("Running %d \n", ticks);
	check_timeouts();

//	kprintf("Check Timeout\n");
	sched_timer();
//	for(;;);
	schedule();
//	kprintf("WOW timer done");

}

/* Create a per process timer */
timer_t timer_create(clockid_t clkid, int flags, struct sigevent *sig)
{
	timer_t tm;
	process_t p = current->proc;

	tm = kmem_cache_alloc(tm_cache, 0);
	if(!tm)
		return (timer_t)-EAGAIN;

	if(clkid != CLOCK_REALTIME)
		return (timer_t)-EINVAL;

	tm->timer_id = alloc_timerid();
	tm->clkid = clkid;
	tm->flag = flags;
	tm->owner = current;

	if(!sig && (flags == TM_TIMER))
	{
		tm->sigv.sigev_notify = SIGEV_SIGNAL;
		tm->sigv.sigev_signo = SIGALRM;
		tm->sigv.sigev_value = tm->timer_id;
	}
	else if(sig)
		tm->sigv = *sig;

	list_add(&p->timer,tm, pproc, timer_t);

	tmhash_insert(tm);

	return tm;
}

int delete_timer(int timer_id)
{
	timer_t tm;

	tm = timer_by_id(timer_id);
	if(!tm)
		return -1;
		
	list_del(&tm->owner->proc->timer, tm, pproc,timer_t);
	kmem_cache_free(tm_cache, tm);
	return 0;
}

/* sleep rqtp time */
int do_nsleep(struct timespec *rqtp, struct timespec *rmtp)
{
	timer_t tmer ;
	long nsecs;

	if((!rqtp->tv_sec && !rqtp->tv_nsec) ||
		((rqtp->tv_nsec < 0) || (rqtp->tv_nsec > 1000000)))
		return -EINVAL;

	/* If sleeping time is less than 10ms sleep non blocking */
	if((!rqtp->tv_sec) && (rqtp->tv_nsec < 10000000))
		delay_nb(rqtp->tv_nsec/1000); /* Microsecond accuracy */

	nsecs = timespec_to_tick(rqtp);
	tmer = timer_create(CLOCK_REALTIME, TM_NOACTION|TM_SLEEP, NULL);
	if((int)tmer < 0)
		return (int)tmer;
	tmer->timeout = ticks + nsecs;
	insert_timer(&tmlist, tmer);
	kprintf("do_nsleep tmout %d tick %d\n", tmer->timeout, ticks);
	current->states = THREAD_INT;
	remove_thread(current);
	schedule();

	return 0;
}

/* Get the resolution of the given clock */
int getresolution(clockid_t clk, struct timespec *tmspec)
{
	/* For Now this is the only supported Clock */
	if(clk != CLOCK_REALTIME)
		return -EINVAL;

	tmspec->tv_sec = 0;
	tmspec->tv_nsec = timer_res[clk];

	return 0;
}

/*
 * Get the time of the given process.
 */
clock_t get_clock(struct tms *tms)
{
	process_t p = current->proc;

	tms->tms_utime = p->user_time;
	tms->tms_stime = p->system_time;

	tms->tms_cutime = get_childs_utime(p);
	tms->tms_cstime = get_childs_stime(p);

	return (tms->tms_utime + tms->tms_stime);
}

/* Destroy all the timers belonging to the process */
void exit_timers(list_t process)
{
	timer_t timer = (timer_t)process->next;

	for(;timer != (timer_t)process; timer=(timer_t)timer->tm_next)
		delete_timer(timer->timer_id);
}

