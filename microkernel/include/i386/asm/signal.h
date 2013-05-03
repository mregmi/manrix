/*
 * ManRiX Microkernel
 * Copyright (C) 2004, 2005
 * 	Manish Regmi (regmi dot manish at gmail dot com) and
 * 	Rajesh R.C   (rajee5283@hotmail.com)
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
 * File:- signal.h
 * 	Architecture dependent signal handling routines.
 */

#ifndef __ASM_SIGNAL_H
#define __ASM_SIGNAL_H

#include <asm/asm.h>
#include <asm/context.h>
#include <asm/ucontext.h>

/* A signal context when signal is being handled */
struct signal_context{
	u32 *jmpaddr;
	int signo;
	siginfo_t *sinfo;
	void *context;
	struct siginfo	info;
	ucontext_t ucontext;
	u16 syscall_return[8];
};

/*static inline int is_blocked(thread_t thr, int signal)
{
	if(thr->sa_mask & signal)
		return 1;
	return 0;
}

static inline void set_sigpending(thread_t thr, int signal)
{
	thr->sigpending |= signal;
}*/

extern void signal_frame(thread_t , sigqueue_t , struct sigaction *, struct stack_regs *);

#endif /* __ASM_SIGNAL_H */
