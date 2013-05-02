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
/*	file: rwlock.h
 *		Various types of lock for Synchronization.
 */


#ifndef __MRX_RWLOCK_H
#define __MRX_RWLOCK_H

#include <mrx/spinlock.h>

struct mrx_rwlock {
	spinlock_t lock;
};

typedef struct mrx_rwlock mrx_rwlock_t;

struct mrx_rwlockattr{
};

typedef struct mrx_rwlockattr mrx_rwlockattr_t;

#include <asm/rwlock.h>

#endif /* __KERNEL_LOCK_H */
