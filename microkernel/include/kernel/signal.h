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
 *
 */

#ifndef __KERNEL_SIGNAL_H
#define __KERNEL_SIGNAL_H

#ifndef __SIGNAL_H
 #include <signal.h>
#endif	/* __SIGNAL_H */

#include <time.h>

#include <kernel/list.h>
#include <kernel/thread.h>
#include <asm/asm.h>

#define MAX_SIGNAL	32

/* Flags used by Kill syscall */
#define TO_THREAD	1	/* signal sent to a thread eg. pthread_kill()*/
#define TO_PROCESS	2	/* signal sent to a process eg. kill() */
#define SIG_REAL	4	/* for sigqueue */

#define next_que(que, ptr)	((ptr) = ((que)->nextq))

#define enqueue_signal(que, process)	\
	list_enqueue(&process->sigqueue, que, queue, sigqueue_t)

struct sigqueue{
	int signal;
	struct siginfo sinfo;
	struct linked_list queue;
};

typedef struct sigqueue *sigqueue_t;

#include <asm/signal.h>

/* add the signals to the mask */
static inline void addmask(sigset_t *set, sigset_t mask)
{
	*set |= mask;
}

/* Delete the signals from the mask */
static inline void delmask(sigset_t *set, sigset_t mask)
{
	*set &= ~mask;
}

/* Set the signal */
static inline void setmask(sigset_t *set, int signal)
{
	set_bit(set, signal);
}


extern int send_signal(struct thread *, int , siginfo_t *);
extern int send_signal_any(struct process * , int , siginfo_t *);
extern void init_signal();
extern void handle_signal(thread_t , struct stack_regs *);
extern void exit_signals(struct process *);
extern void init_signals(struct sigaction []);
extern int sigaction(int , struct sigaction *, struct sigaction *);
extern int sigprocmask(int , const sigset_t *, sigset_t *);
extern int sigsuspend(sigset_t );
extern int sigaltstack(stack_t *, stack_t *);
extern sigqueue_t find_queued(struct process * ,const sigset_t *);
extern int signal_wait(const sigset_t *, siginfo_t *, struct timespec *);

#endif /* __KERNEL_SIGNAL_H */

