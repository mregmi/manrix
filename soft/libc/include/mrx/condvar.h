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


#ifndef __MRX_COND_H
#define __MRX_COND_H

struct mrx_condattr{
};

typedef struct mrx_condattr mrx_condattr_t;

struct mrx_cond{
	volatile int counter;
};

typedef struct mrx_cond mrx_cond_t;

#include <asm/condvar.h>

#endif /* __KERNEL_MUTEX_H */
