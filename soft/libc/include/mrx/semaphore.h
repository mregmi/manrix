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

#include <sys/types.h>
#include <mrx/atomic.h>

#include <mrx/list.h>


struct semaphore{
	volatile int counter;
	volatile int waiting;
	struct linked_list wq;
};

struct semaphore * semaphore_t;



#endif /* __OUROS_SEMAPHORE_H */
