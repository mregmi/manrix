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
/*
 * Signal.h
 */

#ifndef __ASM_SIGNAL_H
#define __ASM_SIGNAL_H

#include <asm/syscalldefs.h>

/* Signal related values. */
#define ID_THREAD			1
#define ID_PROCESS			2
#define SLEEP_INT			4
#define SLEEP_UINT			8
#define WAKE_INT			16
#define WAKE_UINT			32

#define TO_THREAD			1
#define TO_PROCESS			2
#define SIG_REAL			4


static inline int syscall_signal_action(int signal, const struct sigaction *act,
       struct sigaction *oact)
{
	register int ret;
	__asm__ __volatile__(
		"int $0x30"
		:"=a"(ret)
		:"0"(SYS_SIGNAL_ACTION),"b"(signal), "c"(act), "d"(oact)
		);
	return ret;
}

static inline int syscall_kill(int id, int signal, int flags, const union sigval *val)
{
	register int ret;
	__asm__ __volatile__(
		"int $0x30"
		:"=a"(ret)
		:"0"(SYS_KILL),"b"(id), "c"(signal), "d"(flags),"S"(val)
		);
	return ret;
}

static inline int syscall_sig_mask(int how,const sigset_t *set,
       sigset_t *oset)
{
	register int ret;
	__asm__ __volatile__(
		"int $0x30"
		:"=a"(ret)
		:"0"(SYS_SIG_MASK),"b"(how), "c"(set), "d"(oset)
		);
	return ret;
}

static inline int syscall_signal_stack(const stack_t *ss, stack_t *oss)
{
	register int ret;
	__asm__ __volatile__(
		"int $0x30"
		:"=a"(ret)
		:"0"(SYS_SIGNAL_STACK),"b"(ss), "c"(oss)
		);
	return ret;
}

static inline int syscall_signal_suspend(const sigset_t *sigmask)
{
	register int ret;
	__asm__ __volatile__(
		"int $0x30"
		:"=a"(ret)
		:"0"(SYS_SIGNAL_SUSPEND),"b"(*sigmask)
		);
	return ret;
}

static inline int syscall_signal_wait(const sigset_t *__restrict set,
       siginfo_t *__restrict info,
       const struct timespec *__restrict timeout)
{
	register int ret;

	__asm__ __volatile__(
		"int $0x30"
		:"=a"(ret)
		:"0"(SYS_SIGNAL_WAIT),"b"(*set), "c"(info), "d"(timeout)
		);
	return ret;
}

#endif /* __ASM_SIGNAL_H    */
