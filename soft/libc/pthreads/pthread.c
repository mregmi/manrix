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
/*
 * File:- pthread.c
 * 	Posix Threads handling routines.
 */

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>

#include <mrx/pthread-internal.h>
#include <asm/syscall.h>

/*
 * pthread_self - get the calling thread ID
 */
pthread_t pthread_self(void)
{
	return syscall_get_tid();
}

/*
 * pthread_exit - thread termination
 */
void pthread_exit(void *value_ptr)
{
	int *val = (int *)value_ptr;

	syscall_destroy_thread(*val);
}


/*
 * Common Entry for all the threads
 */
void common_entry()
{
	struct pthread_internal *inter;
	int value;

	inter = get_pinternal();
	inter->entry(inter->arg);

	pthread_exit(&value);
}

static inline pthread_t pthread_internal_create(pthread_attr_t *attr,
		       void *(* start_routine)(void *), void *arg)
{
	struct pthread_internal *pint;

	if(attr->stackaddr == NULL)
	{
		attr->stackaddr = malloc(PTHREAD_STACK_DEF);
		if(!attr->stackaddr)
			return -ENOMEM;

		attr->stack_size = PTHREAD_STACK_DEF;
	}

	/* pthread_internal structure will be in the base of the stack */
	pint = (struct pthread_internal *)attr->stackaddr;

	pint->attr = *attr;
	pint->pt_fork.prepare = NULL;
	pint->pt_fork.parent = NULL;
	pint->pt_fork.child = NULL;
	pint->entry = start_routine;
	pint->arg = arg;
	pint->tid = syscall_create_thread(attr, common_entry);

	return pint->tid;
}


/*
 * pthread_create - thread creation
 */
int pthread_create(pthread_t *__restrict thread,
       const pthread_attr_t *__restrict attr,
       void *(* start_routine)(void *), void *__restrict arg)
{
	pthread_attr_t nattr;
	pthread_t tid;

	check_attr(&nattr, attr);


	tid = pthread_internal_create(&nattr, start_routine, arg);
	if(tid < 0)
	{
		if(!attr->stackaddr)
			free(nattr.stackaddr);
		return -tid;
	}

	if(thread)
		*thread = tid;

	return 0;
}

/*
 * pthread_join - wait for thread termination
 */
int pthread_join(pthread_t thread, void **value_ptr)
{
	int ret, value = 0;
	int **val;

	val = (int **)value_ptr;
	ret = syscall_thread_join(thread, &value);
	if(ret < 0)
	{
		return -ret;
	}

	if(value_ptr)
		**val = value;

	return 0;
}


/*
 * pthread_equal - compare thread IDs
 */
int pthread_equal(pthread_t t1, pthread_t t2)
{
	if(t1 == t2)
		return 0;

	return 1;
}


/*
 * pthread_atfork - register fork handlers
 */
int pthread_atfork(void (*prepare)(void), void (*parent)(void),
       void (*child)(void))
{
	struct pthread_internal *inter;

	inter = get_pinternal();

	inter->pt_fork.prepare = prepare;
	inter->pt_fork.parent = parent;
	inter->pt_fork.child = child;

	return 0;
}

