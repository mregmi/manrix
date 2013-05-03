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
/*	file: signal.c
 * 		POSIX style Signal Handling routines.
 */

#include <signal.h>
#include <errno.h>
#include <pthread.h>

#include <asm/signal.h>

/*
 * bsd_signal - replacement for the BSD signal() function
 * for simple applications that are installing a single-argument
 * signal handler function.
 */
void (*bsd_signal(int sig, void (*func)(int)))(int)
{
    struct sigaction act, oact;

    act.sa_handler = func;
    act.sa_flags = SA_RESTART;

    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, sig);
    if (sigaction(sig, &act, &oact) == -1)
        return(SIG_ERR);

    return(oact.sa_handler);
}

/*
 * signal - traditional signal management
 */
void (*signal(int sig, void (*func)(int)))(int)
{
	struct sigaction act, oact;

	act.sa_handler = func;
	act.sa_flags = SA_RESETHAND;

	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, sig);
	if (sigaction(sig, &act, &oact) == -1)
		return(SIG_ERR);

    return(oact.sa_handler);
}

/*
 * sigaction - examine and change a signal action
 */
int sigaction(int sig, const struct sigaction *__restrict act,
       struct sigaction *__restrict oact)
{
	int ret;

	if((sig < 0) || (sig > MAX_SIGNAL))
	{
		errno = EINVAL;
		return -1;
	}

	ret = syscall_signal_action(sig, act, oact);

	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}
	return 0;
}

/*
 * kill - send a signal to a process or a group of processes
 */
int kill(pid_t pid, int sig)
{
	int ret;

	if((sig < 0) || (sig > MAX_SIGNAL))
	{
		errno = EINVAL;
		return -1;
	}
	ret = syscall_kill(pid, sig, TO_PROCESS, NULL);

	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	return 0;
}

/*
 * killpg - send a signal to a process group
 */
int killpg(pid_t pgrp, int sig)
{
	if(pgrp > 1)
		return kill(-pgrp, sig);

	/* Undefined by POSIX */
	/*
	 * we send to all processes  whose process group ID
	 * is equal to the process group ID of the sender
	 */
	else
		return kill(0, sig);
}

/*
 * pthread_kill - send a signal to a thread
 */
int pthread_kill(pthread_t thread, int sig)
{
	int ret;

	if((sig < 0) || (sig > MAX_SIGNAL))
	{
		errno = EINVAL;
		return -1;
	}

	ret = syscall_kill(thread, sig, TO_THREAD, NULL);

	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	return 0;
}

/*
 * raise - send a signal to the executing process
 */
int raise(int sig)
{
	return pthread_kill(pthread_self(), sig);
}

/*
 * sigprocmask - examine and change blocked signals
 */
int sigprocmask(int how, const sigset_t *__restrict set,
       sigset_t *__restrict oset)
{
	int ret;

	ret = syscall_sig_mask(how, set, oset);

	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	return 0;
}

/*
 * pthread_sigmask, examine and change blocked signals
 */
int pthread_sigmask(int how, const sigset_t *__restrict set,
       sigset_t *__restrict oset)
{
	return sigprocmask(how, set, oset);
}

/*
 * sigaltstack - set and get signal alternate stack context
 */
int sigaltstack(const stack_t *__restrict ss, stack_t *__restrict oss)
{
	int ret;

	ret = syscall_signal_stack(ss, oss);

	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	return 0;
}

/*
 * siginterrupt - allow signals to interrupt functions
 */
int siginterrupt(int sig, int flag)
{
	int ret;
	struct sigaction act;

	sigaction(sig, NULL, &act);
	if (flag)
		act.sa_flags &= ~SA_RESTART;
	else
		act.sa_flags |= SA_RESTART;
	ret = sigaction(sig, &act, NULL);

	return ret;
}

/*
 * sigpending - examine pending signals
 */
int sigpending(sigset_t *set)
{
	if(!set)
	{
		errno = EINVAL;
		return -1;
	}

	return sigprocmask(SIG_BLOCK, NULL, set);
}

/*
 * sigsuspend - wait for a signal
 */
int sigsuspend(const sigset_t *sigmask)
{
	int ret;

	ret = syscall_signal_suspend(sigmask);

	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	return 0;
}

/*
 * sigset - signal management
 */
void (*sigset(int sig, void (*disp)(int)))(int)
{
	return signal(sig, disp);
}

/*
 * sighold - signal management
 */
int sighold(int sig)
{
	sigset_t set;

	sigemptyset(&set);
	if(sigaddset(&set, sig) == -1)
			return -1;

	return sigprocmask(SIG_BLOCK, &set, NULL);
}

/*
 * sigrelse - signal management
 */
int sigrelse(int sig)
{
	sigset_t set;

	sigemptyset(&set);
	if(sigaddset(&set, sig) == -1)
		return -1;

	return sigprocmask(SIG_UNBLOCK, &set, NULL);
}

/*
 * sigignore - signal management
 */
int sigignore(int sig)
{
	struct sigaction act;

	act.sa_handler = SIG_IGN;

	return sigaction(sig, &act, NULL);
}

/*
 * sigipause - signal management
 */
int sigpause(int sig)
{
	sigset_t set, oset;

	if(sigpending(&set) == -1)
		return -1;
	oset = set;

	if(sigaddset(&set, sig) == -1)
		return -1;
	if(sigsuspend(&set) == -1)
		return -1;
	return sigprocmask(SIG_SETMASK, &oset, NULL);
}

/*
 * sigwait - wait for queued signals
 */
int sigwait(const sigset_t *__restrict set, int *__restrict sig)
{
	int ret;

	if(set == NULL)
	{
		errno = EINVAL;
		return -1;
	}
	ret = syscall_signal_wait(set, NULL, NULL);

	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}
	*sig = ret;

	return ret;
}

