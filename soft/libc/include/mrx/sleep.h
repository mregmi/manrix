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
/*	file: sleep.h
 *		Task sleep wakeup operations.
 */


#ifndef __OUROS_SLEEP_H
#define __OUROS_SLEEP_H

#ifndef __SYS_TYPES_H
#include <sys/types.h>
#endif

#include <asm/syscall.h>



/* Put the thread into interruptable sleep */
static inline void sleep_int(tid_t tid)
{
	syscall_(tid, 0, SLEEP_INT, NULL);
}


/* Wake the thread from interruptable sleep */
static inline void wake_int(tid_t tid)
{
	syscall_kill(tid, 0, WAKE_INT, NULL);
}

#endif /* __OUROS_SLEEP_H */
