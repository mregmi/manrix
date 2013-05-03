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
/*	file: pthread-spinlock.c
 *		Posix threads Spinlock implementation.
 */

#include <pthread.h>
#include <errno.h>

#include <mrx/spinlock.h>

/*
 * pthread_spin_init - initialize a spin lock object (ADVANCED REALTIME THREADS)
 */
int pthread_spin_init(pthread_spinlock_t *lock, int pshared)
{
	if(!lock)
		return EINVAL;
	spin_lock_init(lock);

	return 0;
}

/*
 * pthread_spin_destroy - destroy a spin lock object (ADVANCED REALTIME THREADS)
 */
int pthread_spin_destroy(pthread_spinlock_t *lock)
{
	if(!lock)
		return EINVAL;

	return 0;
}

/*
 * pthread_spin_lock - lock a spin lock object (ADVANCED REALTIME THREADS)
 */
int pthread_spin_lock(pthread_spinlock_t *lock)
{
	if(!lock)
		return EINVAL;

	spin_lock(lock);

	return 0;
}

/*
 * pthread_spin_trylock - lock a spin lock object (ADVANCED REALTIME THREADS)
 */
int pthread_spin_trylock(pthread_spinlock_t *lock)
{
	if(!lock)
		return EINVAL;

	if(spin_trylock(lock) < 0)
		return EBUSY;

	return 0;
}

/*
 * pthread_spin_unlock - unlock a spin lock object (ADVANCED REALTIME THREADS)
 */
int pthread_spin_unlock(pthread_spinlock_t *lock)
{
	if(!lock)
		return EINVAL;

	spin_unlock(lock);

	return 0;
}
