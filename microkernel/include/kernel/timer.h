/*
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
 */
/*
 * File:- timer.h
 *		Timer functions.
 */
#ifndef __KERNEL_TIMER_H
#define __KERNEL_TIMER_H

#include <signal.h>
#include <time.h>
#include <kernel/list.h>
#include <kernel/thread.h>

#include <asm/timer.h>

#define MAXTIMERS	2

#define TM_SLEEP	1
#define TM_TIMER	2
#define	TM_ONESHOT	4
#define TM_NOACTION	8

/*
 * The Timer Structure.
 */
struct timer{
	struct linked_list	tlist;
#define NEXT_TIMER tlist.next
#define PREV_TIMER tlist.prev
	/* List of timers belonging to same process. */
	struct linked_list	pproc;
#define tm_next		pproc.next
#define tm_prev		pproc.prev

	u64 timeout;
	long nextarm;
	struct sigevent sigv;
	struct thread *owner;
	int flag;
	clockid_t clkid;
	int timer_id;

	struct timer *next;	/* Next hash link */
};

typedef struct timer *timer_t;

static inline long timespec_to_tick(struct timespec *tm)
{
	return ((tm->tv_sec * HZ) + ((tm->tv_nsec/1000000)/HZ));
}

static inline void tick_to_timespec(struct timespec *ts, u64 tick)
{

}

static inline int tick_to_sec(u64 tick)
{
	return 0;
}

/* OS initialization */
extern void time_init();

/* Create a per process timer */
extern timer_t timer_create(clockid_t clkid, int flags, struct sigevent *sig);

/* Delete a Timer */
extern int delete_timer(int timer_id);

/* sleep rqtp time */
extern int do_nsleep(struct timespec *rqtp, struct timespec *rmtp);

/* Find a timer by its id. */
extern timer_t timer_by_id(int timer_id);

extern void insert_timer(timer_t , timer_t );

extern void exit_timers(list_t process);

clock_t get_clock(struct tms *);

#endif /* __KERNEL_TIMER_H */
