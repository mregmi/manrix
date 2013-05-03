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
/*	file: mutex.h
 *		Mutex lock for Synchronization.
 */


#ifndef __ASM_MUTEX_H
#define __ASM_MUTEX_H


/* Lock with Mutex */
static inline void mrx_mutex_lock(mrx_mutex_t *lock)
{
/*	int dummy;

	asm volatile (
		"lock decl %0\n\t"
		"cmpl $0, %0\n\t"
		"jl 2f\n\t"
		"jmp done\n\t"
		"2:	\n\t"
		"pushl %3\n\t"
//		"call mrx_mutex_sleep\n\t"
		"popl %1\n\t"
		"done: \n\t"
		:"=m"(lock->counter),"=m"(dummy)
		:"0"(lock->counter), "m"(* lock)
		:"memory"
	);
	*/
}

/* Try to Lock with Mutex */
static inline int mrx_mutex_trylock(mrx_mutex_t *lock)
{
/*	int dummy;

	asm volatile (
		"lock decl %0\n\t"
		"cmpl $0, %0\n\t"
		:"=m"(lock->counter),"=m"(dummy)
		:"0"(lock->counter), "m"(* lock)
		:"memory"
	);
*/
	return 0;

}

/* Unlock semaphore, also called V or down */
static inline void mrx_mutex_unlock(mrx_mutex_t *lock)
{
/*	int dummy;

	asm volatile(
		"lock incl %0 \n\t"
		"cmpl $0, %0 \n\t"
		"jle 2f\n\t"
		"jmp don \n\t"
		"2: \n\t"
		"pushl %3 \n\t"
//		"call mrx_mutex_wake \n\t"
		"popl %1 \n\t"
		"don: \n\t"
		:"=m"(lock->counter), "=m"(dummy)
		:"0"(lock->counter), "m"(* lock)
		:"memory"
	);*/
}

#endif /* __KERNEL_MUTEX_H */
