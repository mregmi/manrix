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
/*	file: pthread-mutex.c
 *		Posix threads Mutex implementation.
 */

#include <pthread.h>
#include <errno.h>

#include <mrx/lock.h>
#include <mrx/pthread-internal.h>

/*
 * pthread_mutexattr_init - initialize the mutex attributes object
 */
int pthread_mutexattr_init(pthread_mutexattr_t *attr)
{
	if(!attr)
		return EINVAL;

	attr->type = PTHREAD_MUTEX_NORMAL;
	attr->pshared = PTHREAD_PROCESS_SHARED;
	attr->protocol = PTHREAD_PRIO_NONE;

	return 0;
}

/*
 * pthread_mutexattr_destroy - destroy the mutex attributes object
 */
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr)
{
	if(!attr)
		return EINVAL;

	return 0;
}

/*
 * pthread_mutexattr_getprioceiling - get the prioceiling attribute
 * of the mutex attributes object (REALTIME THREADS)
 */
int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *
       __restrict attr, int *__restrict prioceiling)
{
	return ENOSYS;
}


/*
 * pthread_mutexattr_setprioceiling - set the prioceiling attribute
 * of the mutex attributes object (REALTIME THREADS)
 */
int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *attr,
       int prioceiling)
{
	return ENOSYS;
}

/*
 * pthread_mutexattr_getprotocol - get the protocol attribute
 * of the mutex attributes object (REALTIME THREADS)
 */
int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *
       __restrict attr, int *__restrict protocol)
{
	if(!attr)
		return EINVAL;

	*protocol = attr->protocol;

	return 0;
}


/*
 * pthread_mutexattr_setprotocol - set the protocol attribute
 * of the mutex attributes object (REALTIME THREADS)
 */
int pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr,
       int protocol)
{
	if(!attr)
		return EINVAL;

	if((protocol < 1) || (protocol > 3))
		return ENOTSUP;

	attr->protocol = protocol;

	return 0;
}

/*
 * pthread_mutexattr_getpshared - get the process-shared attribute
 */
int pthread_mutexattr_getpshared(const pthread_mutexattr_t *
       __restrict attr, int *__restrict pshared)
{
	if(!attr)
		return EINVAL;

	*pshared = attr->pshared;

	return 0;
}

/*
 * pthread_mutexattr_setpshared - set the process-shared attribute
 */
int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr,
       int pshared)
{
	if(!attr)
		return EINVAL;

	if((pshared < 1) || (pshared > 2))
		return ENOTSUP;

	attr->pshared = pshared;

	return 0;
}

/*
 * pthread_mutexattr_gettype - get the mutex type attribute
 */
int pthread_mutexattr_gettype(const pthread_mutexattr_t *__restrict attr,
       int *__restrict type)
{
	if(!attr)
		return EINVAL;

	*type = attr->type;

	return 0;

}


/*
 * pthread_mutexattr_settype - set the mutex type attribute
 */
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type)
{
	if(!attr)
		return EINVAL;

	if(!(type & PTHREAD_MUTEX_DEFAULT) ||
		!(type & PTHREAD_MUTEX_ERRORCHECK) ||
		!(type & PTHREAD_MUTEX_NORMAL) ||
		!(type & PTHREAD_MUTEX_RECURSIVE))
		return ENOTSUP;

	attr->pshared = type;

	return 0;
}


/*
 * pthread_mutex_init - initialize a mutex
 */
int pthread_mutex_init(pthread_mutex_t *__restrict mutex,
       const pthread_mutexattr_t *__restrict attr)
{
	int ret;

	if(!mutex)
		return EINVAL;

	if(!attr)
		pthread_mutexattr_init(&mutex->mattr);

	ret = syscall_sync_create(SYNC_MUTEX);
	if(ret < 0)
		return -ret;

	mutex->sync_id = ret;

	return 0;

}


/*
 * pthread_mutex_destroy - destroy a mutex
 */
int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
	int ret;

	if(!mutex)
		return EINVAL;

	ret = syscall_sync_destroy(mutex->sync_id);
	if(ret < 0)
		return -ret;

	return 0;
}

/*
 * pthread_mutex_lock - lock a mutex
 */
int pthread_mutex_lock(pthread_mutex_t *mutex)
{
//	int ret;

	if(!mutex)
		return EINVAL;

	mrx_mutex_lock(mutex);

	return 0;
}

/*
 * pthread_mutex_trylock - lock a mutex
 */
int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
//	int ret;

	if(!mutex)
		return EINVAL;

	return mrx_mutex_trylock(mutex);

}

/*
 * pthread_mutex_unlock - unlock a mutex
 */
int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
//	int ret;

	if(!mutex)
		return EINVAL;

	mrx_mutex_unlock(mutex);

	return 0;
}

/*
 * pthread_mutex_timedlock - lock a mutex (ADVANCED REALTIME)
 */
int pthread_mutex_timedlock(pthread_mutex_t *__restrict mutex,
       const struct timespec *__restrict abs_timeout)
{
	return ENOSYS;
}
