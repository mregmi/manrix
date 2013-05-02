/**
 * ManRiX microkernel
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
/**	file: kpreempt.h
 *		Kernel Preemption Primitives. Kernel preemption is disabled by
 *  incrementing the per thread kpreempt value. When Preemption is disabled,
 *  interrupt is enabled but the schedular does not run.
 **/


#ifndef __KERNEL_KPREEMPT_H
#define __KERNEL_KPREEMPT_H

#include <kernel/thread.h>
#include <kernel/config.h>

#ifndef MANRIX_KPREEMPT

/* Check if preemption is disabled */
static inline int on_kpreempt()
{
	return 0;
}

#define kpreempt_disable()		do{}while(0)
#define kpreempt_enable()		do{}while(0)

#else

#define kpreempt_disable()		(current->kpreempt++)
#define kpreempt_enable()		(current->kpreempt--)

/* Check if preemption is disabled */
static inline int on_kpreempt()
{
	if(current->kpreempt != 0)
		return 1;
	return 0;
}


#endif /* MANRIX_KPREEMPT */
#endif /* __KERNEL_KPREEMPT_H */
