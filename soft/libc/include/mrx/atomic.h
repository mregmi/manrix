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
/*	file: atomic.h
 *		Atomic Operations implementation.
 *	SMP safe.
 */


#ifndef __OUROS_ATOMIC_H
#define __OUROS_ATOMIC_H

typedef volatile int atomic_t;

#define atomic_init(a)	{a = 0};

#define atomic_set(a, val)	{a = val};

/* All other things are architecture dependent */
#include <asm/atomic.h>

#endif /* __OUROS_SEMAPHORE_H */
