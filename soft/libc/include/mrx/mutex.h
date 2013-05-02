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


#ifndef __KERNEL_MUTEX_H
#define __KERNEL_MUTEX_H

#define MUTEX_NORMAL		1
#define MUTEX_ERRORCHECK	2
#define MUTEX_RECURSIVE		4
#define MUTEX_DEFAULT		8


#define MUTEX_INIT_UNLOCKED		{{MUTEX_DEFAULT, 0, 0}, 0, 0}

struct mrx_mutexattr{
	int type;
	int pshared;
	int protocol;
};

typedef struct mrx_mutexattr mrx_mutexattr_t;

struct mrx_mutex{
	struct mrx_mutexattr mattr;
	volatile int counter;
	int sync_id;
};

typedef struct mrx_mutex mrx_mutex_t;

#include <asm/mutex.h>

#endif /* __KERNEL_MUTEX_H */
