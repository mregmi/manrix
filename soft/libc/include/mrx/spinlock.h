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
/*	file: spinlock.h
 *		Spin Lock implementation.
 */


#ifndef __KERNEL_SPINLOCK_H
#define __KERNEL_SPINLOCK_H

#define SPIN_LOCK_UNLOCKED	{1}
#define SPIN_LOCK_LOCKED	{-1}

#define spin_lock_init(s)	(s->lock = 1)


/* Spinlock Structure */
struct spinlock{
	volatile int lock;
};

typedef struct spinlock spinlock_t;

/* Implementation is architecture dependent. */
#include <asm/spinlock.h>

#endif /* __KERNEL_SPINLOCK_H */
