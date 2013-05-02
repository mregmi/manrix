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
/*	file: barrier.h
 *		Barrier for Synchronization.
 */


#ifndef __MRX_BARRIER_H
#define __MRX_BARRIER_H

#include <mrx/spinlock.h>

struct mrx_barrier{
	spinlock_t lock;
};

typedef struct mrx_barrier mrx_barrier_t;

struct mrx_barrierattr{
};

typedef struct mrx_barrierattr mrx_barrierattr_t;

#include <asm/barrier.h>

#endif /* __MRX_BARRIER_H */
