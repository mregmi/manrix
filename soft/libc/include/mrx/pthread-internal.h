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
 * Posix Threads declarations.
 */

#ifndef __PTHREAD_INTERNAL_H
#define __PTHREAD_INTERNAL_H

#include <pthread.h>
#include <asm/syscall.h>

#define INHERITSCHED	0x03
#define CONTENTIONSCOPE	0x0C
#define DETACHSTATE		0x30



/* Fork Handlers. There is only one handler. */
struct fork_handler{
	void (* prepare)(void);
	void (* parent)(void);
	void (* child)(void);
};

/* Each Thread has this structure in user space */
struct pthread_internal{
	struct pthread_attr	attr;
	struct fork_handler pt_fork;
	void *(* entry)(void *);
	void *arg;
	pthread_t tid;
};

static inline struct pthread_internal *get_pinternal()
{
	struct pthread_internal *internal;

	internal = (struct pthread_internal *)syscall_get_ustack();

	return internal;
}

static inline void check_attr(pthread_attr_t *nattr, const pthread_attr_t *attr)
{
	struct pthread_internal *inter;

	if(attr == NULL)
	{
		pthread_attr_init(nattr);
		return;
	}

	if(attr->flags & PTHREAD_EXPLICIT_SCHED)
	{
		*nattr = *attr;
	}
	else
	{
		inter = get_pinternal();
		*nattr = inter->attr;
		nattr->stackaddr = NULL;
		nattr->stack_size = 0;
	}
}

#endif /* __PTHREAD_INTERNAL_H */

