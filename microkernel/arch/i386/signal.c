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
 * File:- signal.c
 *	 Architecture dependent signal handling routines.
 *
 */

#include <string.h>

#include <kernel/signal.h>
#include <kernel/kernel_stdio.h>
#include <kernel/vm.h>
#include <kernel/scheduling.h>
#include <kernel/irq.h>


/*
 * create a signal context for handling signals.
 * make sure you are holding the signal lock.
 */
void signal_frame(thread_t thr, sigqueue_t sq, struct sigaction *action, struct stack_regs *regs)
{
	struct signal_context *sigframe;


	if(action->sa_flags & SA_ONSTACK)
	{
		if(thr->proc->ss.ss_flags == SS_DISABLE)
			goto usedefault;
		sigframe = (struct signal_context *)(thr->proc->ss.ss_sp \
			+ thr->proc->ss.ss_size - sizeof(struct signal_context));
		thr->proc->ss.ss_flags = SS_ONSTACK;
	}
	else
	{
usedefault:
	sigframe = (struct signal_context *)(regs->user_esp - sizeof(struct signal_context));
	}

	put_user_long((u32)&sigframe->syscall_return, (unsigned long *)&sigframe->jmpaddr);
	put_user_word(0xb890, (unsigned short *)&sigframe->syscall_return[0]); /* nop; movl $0, %%eax */
 	put_user_long(0x0, (unsigned long *)&sigframe->syscall_return[1]);
	put_user_word(0x30cd, (unsigned short *)(&sigframe->syscall_return[3]));	/* int $0x30 */
	ker_to_usr((char *)&sigframe->ucontext.uc_mcontext, (char *)regs, sizeof(struct stack_regs));

	put_user_long(sq->signal, (unsigned long *)&sigframe->signo);
	put_user_long(thr->sa_mask, (unsigned long *)&sigframe->ucontext.uc_sigmask);

	/* this is for sa_sigaction only */
	if(action->sa_mask & SA_SIGINFO)
	{
		put_user_long((u32)&sigframe->info, (unsigned long *)&sigframe->sinfo);
		put_user_long((u32)&sigframe->ucontext, (unsigned long *)&sigframe->context);
		ker_to_usr((char *)&sigframe->info, (char *)&sq->sinfo, sizeof(struct siginfo));
	}

	addmask(&thr->sa_mask, action->sa_mask);
	regs->user_eip = (u32)action->sa_sigaction;
	regs->user_esp = (u32)sigframe;

	if(action->sa_flags & SA_RESETHAND)
	{
		action->sa_mask &= ~SA_SIGINFO;
		action->sa_handler = SIG_DFL;
	}
}

/*
 * Check if We received any signal;
 * or IRQ(if an IRQ handler) and
 * also check if scheduling is pending.
 */
void check_signal(struct stack_regs *regs)
{
	thread_t thr = current;

	if(thr->schedule)
		schedule();

	/* If it is a ISR thread and received the IRQ. */
	if(thr->context.irqflag & IRQ_REQUEST)
	{
	//	handle_IRQ_event(thr, regs);
		return ;
	}

	if(thr->sigpending)
		handle_signal(thr, regs);
}

/* restore the signal context after returning from signal */
int syscall_signal_return(struct stack_regs *r)
{
	struct signal_context *sigframe;
	process_t p = current->proc;
	struct stack_regs *regs = r;
//	sigset_t saved;
//	unsigned long s;

	sigframe = (struct signal_context *)(regs->user_esp - 4);
	usr_to_ker((void *)regs, (void *)&sigframe->ucontext.uc_mcontext, sizeof(struct stack_regs));

//	regs->user_esp += sizeof(struct irq_frame);
	if(current->context.irqflag & IRQ_REQUEST)
	{
		current->context.irqflag &= ~ON_IRQHAND;
		current->context.irqflag &= ~IRQ_REQUEST;
		return 0;
	}

	if(p->status != ON_SIGHAND)
		return -1;

	current->sa_mask = get_user_long((unsigned long *)&sigframe->ucontext.uc_sigmask);
	p->ss.ss_flags = 0;

	p->status = PROCESS_ACTIVE;
	return 0;
}


/*
 * Syscall :- signal action
 * Define action to take on receipt of a signal.
 * ebx = signal number
 * ecx = pointer to act structure
 * edx = pointer to oact structure
 */
int syscall_signal_action(struct stack_regs *regs)
{
	int ret, signal;
	struct sigaction act, oact;
	struct sigaction *poact = NULL;

	signal = regs->ebx;
//	kprintf(" saction ");
	if(regs->ecx)
		usr_to_ker((void *)&act, (void *)regs->ecx, sizeof(struct sigaction));
	if(regs->edx)
		poact = &oact;

	ret = sigaction(signal, &act, poact);

	if(poact)
		ker_to_usr((void *)regs->edx, (void *)poact, sizeof(struct sigaction));

	return ret;
}

/*
 * Change blocked signal mask of a thread.
 * ebx = pointer to set structure
 * ecx = pointer to oset structure
 * edx = how field
 */
int syscall_sig_mask(struct stack_regs *regs)
{
	int ret;
	sigset_t *set = NULL, *oset = NULL;

	if(regs->ecx)
		set = (sigset_t *)regs->ecx;
	if(regs->edx)
		oset = (sigset_t *)regs->edx;

	ret = sigprocmask(regs->ebx, set, oset);

	return ret;
}

/*
 * send a signal to a particular thread or process
 * ebx = pid or tid
 * ecx = signal number
 * edx = flags (see kernel/signal.h)
 * esi = pointer to sigval (used by sigqueue)
 */
int syscall_kill(struct stack_regs *regs)
{
	int ret = 0;
	thread_t thr;
	process_t p;
	struct siginfo info, *pinfo = NULL;

//	print_regs(regs);

	if((regs->ecx < 0) || (regs->ecx > MAX_SIGNAL))
		return -EINVAL;

	if((regs->edx & SIG_REAL) && regs->esi)
	{
		pinfo = &info;
		usr_to_ker((void *)&pinfo->si_value, (void *)regs->esi, sizeof(union sigval));
	}

	if(regs->edx & TO_THREAD)
	{
		thr = find_by_tid(regs->ebx);
//	kprintf("KILL called by %d sig %d thr %x", regs->ebx, regs->ecx, thr);
		if(!thr)
			goto invid;
		send_signal(thr, regs->ecx, NULL);
	}
	else
	{
		if(regs->ebx > 0)
		{
			p = find_by_pid(regs->ebx);
			if(!p)
				goto invid;
			send_signal_any(p, regs->ecx, pinfo);
		}
	}

	return ret;
invid:
//	kprintf("Invalid KILL %d by tid %d to tid %d. ", regs->ecx, current->tid, regs->ebx);
	return -ESRCH;
}

/*
 * Suspend the thread until any signal is delivered.
 * ebx = signal set (used by sigsuspend)
 */

int syscall_sigsuspend(struct stack_regs *regs)
{
	int ret;
	ret = sigsuspend(regs->ebx);

	return ret;
}

/*
 * provide an alternate stack for signal handling.
 * ebx = ss
 * ecx = oss
 */
int syscall_signal_stack(struct stack_regs *regs)
{
	int ret;
	stack_t ss, oss, *pss=NULL, *poss = NULL;

	if(regs->ebx)
	{
		pss = &ss;
		usr_to_ker((void *)pss, (void *)regs->ebx, sizeof(stack_t));
	}
	if(regs->ecx)
		poss = &oss;

	ret = sigaltstack(pss, poss);

	if(regs->ecx)
		ker_to_usr((void *)regs->ecx, (void *)poss, sizeof(stack_t));

	return ret;
}

/*
 * Wait for a Signal to arrive.
 */
int syscall_signal_wait(struct stack_regs *regs)
{
	struct siginfo info, *pinfo = NULL;
	struct timespec spec, *pspec = NULL;
	int ret;

	if(regs->edx)
	{
		pspec = &spec;
 		usr_to_ker((void *)pspec, (void *)regs->edx, sizeof(struct timespec));
	}
	if(regs->ecx)
		pinfo = &info;

	ret = signal_wait(&regs->ebx, &info, pspec);
	if(ret < 0)
		return ret;
	if(pinfo != NULL)
		ker_to_usr((void *)regs->ecx, (void *)&info, sizeof(struct siginfo));

	return ret;
}
