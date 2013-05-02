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
 * File: signal.c
 *
 * Architecture independent signal handling routines.
 *
 */

#include <kernel/scheduling.h>
#include <kernel/kernel_stdio.h>
#include <kernel/signal.h>
#include <kernel/vm.h>

#include <asm/asm.h>

/* A slab Cache for signal queuing */
kmem_cache_t sigqueue_cache;

/* Check if Signal Queue is empty. */
static inline int is_queued(struct process *p)
{
	if(p->nextq == &p->sigqueue)
		return 0;
	return 1;
}

static inline int is_blocked(thread_t thread, int signal)
{
	if(check_set(thread->sa_mask, signal))
		return 1;
	return 0;
}

/* Allocate a new Signal Queue. */
static inline sigqueue_t allocate_sigqueue()
{
	register sigqueue_t sq;
	sq = kmem_cache_alloc(sigqueue_cache, 0);

	return sq;
}

/* Deallocate a signal Queue. */
static inline void deallocate_sigqueue(sigqueue_t sq)
{
	kmem_cache_free(sigqueue_cache, sq);
}

/* Dequeue the unblocked signal */
static inline sigqueue_t dequeue_signal(thread_t thr)
{
	sigqueue_t sq;
	process_t p = thr->proc;

	for(sq = p->nextq; sq != (sigqueue_t)&p->sigqueue; sq = sq->queue.next)
	{
		if(is_blocked(thr, sq->signal))
			continue;
		kprintf("Dequeuing %d ", sq->signal);
		list_del(&p->sigqueue, sq, queue, sigqueue_t);
//		if(!sq)
//			MANRIX_BUG();

		return sq;
	}

	return NULL;
}

/* check if any signal is pending */
static inline int is_pending(thread_t thr)
{
	process_t p = thr->proc;
	sigqueue_t sq;

	for(sq=p->nextq ; sq->queue.next != &p->sigqueue ; sq=sq->queue.next)
	{
		if(is_blocked(thr, sq->signal))
			continue;
		return 1;
	}

	return 0;
}

/* Initialize Signals */
void init_signal()
{
	sigqueue_cache = kmem_cache_create("Signal Queue", sizeof(struct sigqueue), 32, NULL, NULL);
}

void handle_signal(thread_t thr, struct stack_regs *regs)
{
	struct sigqueue *q;
	struct sigaction *act;
	process_t p = thr->proc;

	q = dequeue_signal(thr);
	if(!q)
	{
		goto nosig;
	}
	act = &p->action[q->signal - 1];
	kprintf("sig %x", (u32)act->sa_handler);
	switch((u32)act->sa_handler)
	{
		case SIG_IGN:
			// Not posssible because ignored signals are not queued. */
			break;

		case SIG_DFL:
			switch(q->signal)
			{
				case SIGCHLD:
					break;
				/* Not implemented. Ignore for Now	*/
				case SIGCONT:
				case SIGTSTP:
				case SIGTTIN:
				case SIGTTOU:
				case SIGSTOP:
					break;
				default:
					p->exit_status |= WIFSIG;
					p->exit_status |= (q->signal << 8) & WSIGNAL;
					thr->states = PROCESS_EXITING;
					syscall_exit(NULL, 1);
			}
			break;
		default:
			/* architecture specfic */
			p->status = ON_SIGHAND;
			signal_frame(thr, q, act, regs);
	}
	deallocate_sigqueue(q);
nosig:
	if(!is_pending(thr))
		thr->sigpending = 0;
}



int send_signal(thread_t thr, int signal, siginfo_t *sinfo)
{
	sigqueue_t sq;
	struct sigaction *act;

	if((signal < 0) || (signal > MAX_SIGNAL))
		return -EINVAL;

	if(is_blocked(thr, signal))
		goto noque;

	act = &thr->proc->action[signal-1];
	if((int)act->sa_handler == SIG_IGN)
		goto noque;
	sq = allocate_sigqueue();
	if(!sq)
		return -EAGAIN;
	if(sinfo)
		sq->sinfo = *sinfo;
	sq->signal = signal;

	enqueue_signal(sq, thr->proc);

	thr->sigpending = 1;
noque:
	if(thr->states == THREAD_INT)
		wakeup(thr);
	return 0;
}

/*
 * It is used when a signal is sent to a process.
 * The signal is sent to the thread which has this signal unblocked.
 */
int send_signal_any(process_t p, int signal, siginfo_t *sinfo)
{
	thread_t thr = (thread_t)p->threads.next;

	for(;thr != (thread_t)&p->threads; thr=(thread_t)thr->threadnext)
	{
		if(!is_blocked(thr, signal))
		{
			kprintf("send_signal_any: thread found %d ", thr->tid);
			return send_signal(thr, signal, sinfo);
		}
	}
	return 0;
}

/* Destroy all the signal queues of the process */
void exit_signals(process_t process)
{
	sigqueue_t sq = process->nextq;

	for(;sq != (sigqueue_t)&process->sigqueue; sq=(sigqueue_t)sq->queue.next)
		deallocate_sigqueue(sq);
}

/* Initialize the signals to default */
void init_signals(struct sigaction action[])
{
	int index;

	for(index = 0; index < MAX_SIGNAL; index++)
	{
		action[index].sa_mask = 0;
		action[index].sa_flags = 0;
		action[index].sa_handler = (void *)SIG_DFL;
	}
//	action[SIGCHLD - 1].sa_handler = (void *)SIG_IGN;
}

/* Examine or specify the action to be taken when signal occurs */
int sigaction(int signal, struct sigaction *act, struct sigaction *oact)
{
	//thread_t thr = current;
	process_t p = current->proc;

	if(signal <= 0 || signal > MAX_SIGNAL)
		return -EINVAL;
	if(signal == SIGKILL || signal == SIGSTOP)
		return -EINVAL;
	if(oact)
		*oact = p->action[signal-1];
	if(!act)
		return 0;

	p->action[signal-1] = *act;
//	kprintf("signal action completed %d", act->sa_handler);

	return 0;
}


int sigprocmask(int how, const sigset_t *set, sigset_t *oset)
{
	thread_t thr = current;
	if(oset)
		*oset = thr->sa_mask;
	if(!set)
		return 0;
	switch (how)
	{
		case SIG_BLOCK:
			addmask(&thr->sa_mask, *set);
			break;
		case SIG_UNBLOCK:
			delmask(&thr->sa_mask, *set);
			break;
		case SIG_SETMASK:
			thr->sa_mask = *set;
			break;
		default:
			return -EINVAL;
	}
	return 0;
}

/* Suspend the thread until any signal is delivered. */
int sigsuspend(sigset_t set)
{
	thread_t thr = current;

	thr->sa_mask = set;
	thr->states = THREAD_INT;
	remove_thread(thr);
	schedule();

	return 0;
}

int sigaltstack(stack_t *ss, stack_t *oss)
{
	process_t p = current->proc;

	if(oss)
		*oss = p->ss;

	if(!ss)
		return 0;

	if(ss->ss_flags & SS_DISABLE)
		return -EINVAL;

	if(p->ss.ss_flags & SS_ONSTACK)
		return -EPERM;

	if(p->ss.ss_size < MINSIGSTKSZ)
		return -ENOMEM;

	p->ss = *ss;

	return 0;
}

static inline int set_has_signal(sigset_t set, int signal)
{
	if(set & signal)
		return 1;
	return 0;
}

/* find a signal from the set queued */
sigqueue_t find_queued(process_t p,const sigset_t *set)
{
	sigqueue_t sq;

	if(!is_queued(p))
		return NULL;

	sq = (sigqueue_t)p->sigqueue.next;
	while(sq != (sigqueue_t)&p->sigqueue)
	{
		if(set_has_signal(*set, sq->signal))
			return sq;
		sq = (sigqueue_t) sq->queue.next;
	}

	return NULL;
}


int signal_wait(const sigset_t *set, siginfo_t *info, struct timespec *timeout)
{
	thread_t thr = current;
	sigqueue_t	queue;
	int err = -1;


	queue = find_queued(thr->proc, set);
	if(queue != NULL)
	{

		if(timeout != NULL)
			err = do_nsleep(timeout, NULL);
		else
		{
intr:
			sleep_interruptible(thr);
		}
		queue = find_queued(thr->proc, set);
		if(queue == NULL)
			return -EAGAIN;
	}
	else
	{
		goto intr;
	}

	if(err < 0)
		return err;

	*info = queue->sinfo;
	list_del(&thr->proc->sigqueue, queue, queue, sigqueue_t);
	if(!queue)
		return -EINVAL;

	deallocate_sigqueue(queue);

	if(!is_pending(thr))
		thr->sigpending = 0;

	return 0;
}

