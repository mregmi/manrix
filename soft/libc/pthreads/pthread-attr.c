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
 */
/*
 * File:- pthread-attr.c
 * 	Posix Threads attribute handling routines.
 */

#include <pthread.h>
#include <sched.h>
#include <errno.h>

#include <mrx/pthread-internal.h>
#include "pthread.h"

/*
 * pthread_attr_init - destroy the thread attributes object
 */
int pthread_attr_init(pthread_attr_t *attr)
{
	attr->flags = PTHREAD_SCOPE_SYSTEM | PTHREAD_CREATE_JOINABLE | PTHREAD_EXPLICIT_SCHED;
	attr->policy = SCHED_OTHER;
	attr->prio.sched_priority = PRIO_OTHER_DEF;
	attr->stackaddr = NULL;
	attr->stack_size = 0;

	return 0;
}

/*
 * pthread_attr_destroy - destroy the thread attributes object
 */
int pthread_attr_destroy(pthread_attr_t *attr)
{
	return 0;
}

/*
 * pthread_attr_getinheritsched -
 * get the inheritsched attribute (REALTIME THREADS)
 */
int pthread_attr_getinheritsched(const pthread_attr_t *__restrict attr,
       int *__restrict inheritsched)
{
	*inheritsched = attr->flags & INHERITSCHED;

	return 0;
}


/*
 * pthread_attr_setinheritsched -
 * set the inheritsched attribute (REALTIME THREADS)
 */
int pthread_attr_setinheritsched(pthread_attr_t *attr,
       int inheritsched)
{
	attr->flags |= inheritsched;

	return 0;
}

/*
 * pthread_attr_getschedparam - get the schedparam attribute
 */
int pthread_attr_getschedparam(const pthread_attr_t *__restrict attr,
       struct sched_param *__restrict param)
{
	*param = attr->prio;

	return 0;
}

/*
 * pthread_attr_setschedparam - set the schedparam attribute
 */
int pthread_attr_setschedparam(pthread_attr_t *__restrict attr,
       const struct sched_param *__restrict param)
{
	if(param->sched_priority < MIN_PRIO
		&& param->sched_priority > MAX_PRIO)
		return EINVAL;

	attr->prio = *param;

	return 0;
}

/*
 * pthread_attr_getschedpolicy -
 * get the schedpolicy attribute (REALTIME THREADS)
 */
int pthread_attr_getschedpolicy(const pthread_attr_t *__restrict attr,
       int *__restrict policy)
{
	*policy = attr->policy;

	return 0;
}

/*
 * pthread_attr_setschedpolicy -
 * set the schedpolicy attribute (REALTIME THREADS)
 */
int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy)
{
	if( (policy != SCHED_FIFO) &&
		(policy != SCHED_RR) &&
		(policy != SCHED_OTHER))
		return EINVAL;

	attr->policy = policy;

	return 0;
}

/*
 * pthread_attr_getscope - get the contentionscope attribute (REALTIME THREADS)
 */
int pthread_attr_getscope(const pthread_attr_t *__restrict attr,
       int *__restrict contentionscope)
{
	*contentionscope = attr->flags & CONTENTIONSCOPE;

	return 0;
}

/*
 * pthread_attr_setscope - set the contentionscope attribute (REALTIME THREADS)
 */
int pthread_attr_setscope(pthread_attr_t *attr, int contentionscope)
{
	if((contentionscope != PTHREAD_SCOPE_SYSTEM) &&
		(contentionscope != PTHREAD_SCOPE_PROCESS))
		return EINVAL;

	attr->flags |= contentionscope;

	return 0;
}

/*
 * pthread_attr_getstackaddr - get the stackaddr attribute
 */
int pthread_attr_getstackaddr(const pthread_attr_t *__restrict attr,
       void **__restrict stackaddr)
{
	unsigned long **stk = (unsigned long **)stackaddr;
	**stk = (unsigned long)&attr->stackaddr;

	return 0;
}

/*
 * pthread_attr_setstackaddr - set the stackaddr attribute
 */
int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackaddr)
{
	attr->stackaddr = stackaddr;

	return 0;
}

/*
 * pthread_attr_getstack - get stack attributes
 */
int pthread_attr_getstack(const pthread_attr_t *__restrict attr,
       void **__restrict stackaddr, size_t *__restrict stacksize)
{
	pthread_attr_getstackaddr(attr, stackaddr);
	*stacksize = attr->stack_size;

	return 0;
}

/*
 * pthread_attr_setstack - set stack attributes
 */
int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr,
       size_t stacksize)
{
	if(stacksize < PTHREAD_STACK_MIN)
		return EINVAL;

	pthread_attr_setstackaddr(attr, stackaddr);
	attr->stack_size = stacksize;

	return 0;
}

/*
 * pthread_attr_getdetachstate - get and set the detachstate attribute
 */
int pthread_attr_getdetachstate(const pthread_attr_t *attr,
       int *detachstate)
{
	*detachstate = attr->flags & DETACHSTATE;

	return 0;
}

/*
 * pthread_attr_getdetachstate - get and set the detachstate attribute
 */
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate)
{
	attr->flags |= detachstate;

	return 0;
}

/*
 * pthread_attr_getguardsize - get the thread guardsize attribute
 */
int pthread_attr_getguardsize(const pthread_attr_t *__restrict attr,
       size_t *__restrict guardsize)
{
	return 0;
}

/*
 * pthread_attr_setguardsize - set the thread guardsize attribute
 */
int pthread_attr_setguardsize(pthread_attr_t *attr,
       size_t guardsize)
{
	return 0;
}

