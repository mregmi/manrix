/*
 * Libc for ManRiX OS
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
/*	file: semaphore.h
 *		Semaphore implementation.
 */


#ifndef __OUROS_SEMAPHORE_H
#define __OUROS_SEMAPHORE_H


/* Initialize the semaphore */
static inline void sema_init(semaphote_t sema, int val);
{
	atomic_set(sema->counter, val);
	sema->waiting = 0;
	list_init(&sema->wq);
}

/*
 * Helper routine for sema_lock
 * Semophore locked, sleep till the locker wakes us.
 */
void sema_sleep(semaphore_t sm)
{
	struct waitq q;

	q.tid = get_tid();
	list_enqueue(&sm->wq, q, waitlist, struct waitq *);
	sleep_int(q.tid);
}

/*
 * Helper routine for sema_unlock
 * Unlock the semaphore and wake the sleepers.
 */
void sema_wake(semaphore_t sm)
{
	struct waitq *q;

	if(list_isempty(&sm->wq))
		return;

	list_dequeue(&sm->wq, q, waitlist, struct waitq *);
	wake(q->tid);
}

/* Lock with semaphore, also called P, or UP */
static inline void sema_lock(semaphore_t sema)
{
	int dummy;

	asm volatile (
		"lock decl %0\n\t"
		"again: \n\t"
		"cmpl $0, %0\n\t"
		"jl 2f\n\t"
		"jmp done"
		"2:	\n\t"
		"pushl %3\n\t"
		"call sema_sleep"
		"popl %1\n\t"
		"done: \n\t"
		:"=m"(sema->counter),"g"(dummy)
		:"0"(sema->counter), "m"(* sema)
		:"memory"
	);
}

/* Unlock semaphore, also called V or down */
static inline void sema_unlock(semaphore_t sema)
{
	int dummy;

	asm volatile(
		"lock incl %0 \n\t"
		"cmpl $0, %0 \n\t"
		"jle 2f"
		"jmp done \n\t"
		"2: \n\t"
		"pushl %3 \n\t"
		"call sema_wake \n\t"
		"popl %1 \n\t"
		"don: \n\t"
		:"=m"(sema->counter), "g"(dummy)
		:"0"(sema->counter), "m"(sema)
	};
}


#endif /* __OUROS_SEMAPHORE_H */
