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
 * File:- sigrt.c
 * 	This file handles POSIX 1003b calls i.e. realtime signals.
 *
 */
#include <signal.h>
#include <errno.h>

#include <asm/signal.h>

/*
 * sigqueue - queue a signal to a process (REALTIME)
 */
int sigqueue(pid_t pid, int signo, const union sigval value)
{
	int ret;

	if((signo < 0) || (signo > MAX_SIGNAL))
	{
		errno = EINVAL;
		return -1;
	}

	ret = syscall_kill(pid, signo, TO_PROCESS|SIG_REAL, &value);
	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	return 0;
}

/*
 * sigwaitinfo - wait for queued signals (REALTIME)
 */
int sigwaitinfo(const sigset_t *__restrict set,
       siginfo_t *__restrict info)
{
	int ret;

	if(set == NULL)
	{
		errno = EINVAL;
		return -1;
	}
	ret = syscall_signal_wait(set, info, NULL);

	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	return ret;
}

/*
 * sigtimedwait - wait for queued signals (REALTIME)
 */
int sigtimedwait(const sigset_t *__restrict set,
       siginfo_t *__restrict info,
       const struct timespec *__restrict timeout)
{
	int ret;

	if(set == NULL)
	{
		errno = EINVAL;
		return -1;
	}
	ret = syscall_signal_wait(set, info, timeout);

	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	return ret;
}
