/**
 * ManRiX Microkernel
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
 **/
/**	file: spinlock.h
 *		Spin Lock implementation.
 **/


#ifndef __KERNEL_SPINLOCK_H
#define __KERNEL_SPINLOCK_H

#include <kernel/config.h>
#include <kernel/kpreempt.h>

#include <asm/asm.h>

#define SPIN_LOCK_UNLOCKED	{1}
#define SPIN_LOCK_LOCKED	{0}

/* Spinlock Structure */
struct spinlock{
	volatile int lock;
};

typedef struct spinlock spinlock_t;


#define SPIN_LOCK_INIT(s)	((s)->lock = 1)

#ifndef MANRIX_SMP

 #define spin_lock_init(s)		do{ }while(0)
 #define spin_lock(s)			{ kpreempt_disable(); }
 #define spin_unlock(s)			{ kpreempt_enable(); }
 #define spin_trylock(s)		do{ }while(0)

#else	/* SMP Microkernel */

 #define spin_lock_init(s)	(s->lock = 1)
/* Implementation is architecture dependent. */
 #include <asm/spinlock.h>

#endif	/* MANRIX_SMP */

#define spin_lock_cli(s)		{ CLI(); spin_lock(s); }
#define spin_unlock_cli(s)		{ STI(); spin_unlock(s); }

#endif /* __KERNEL_SPINLOCK_H */
