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
 * File:- sched.c
 * 	This file handles POSIX scheduling functions.
 *
 */

#include <sched.h>
#include <errno.h>

#include <asm/syscall.h>
#include <asm/signal.h>

/*
 * sched_get_priority_max - get priority limits (REALTIME)
 * For now it is static.
 */
int sched_get_priority_max(int policy)
{
	int max;

	switch(policy)
	{
		case SCHED_OTHER:
			max = PRIO_OTHER_MAX;
			break;
		case SCHED_FIFO:
		case SCHED_RR:
			max = PRIO_RT_MAX;
			break;
		default:
			errno = EINVAL;
			return -1;
	}
	return max;
}

/*
 * sched_get_priority_max - get priority limits (REALTIME)
 * For now it is static.
 */

int sched_get_priority_min(int policy)
{
	int max;

	switch(policy)
	{
		case SCHED_OTHER:
			max = PRIO_OTHER_MIN;
			break;
		case SCHED_FIFO:
		case SCHED_RR:
			max = PRIO_RT_MIN;
			break;
		default:
			errno = EINVAL;
			return -1;
	}
	return max;
}


/*
 * sched_getparam - get scheduling parameters (REALTIME)
 */
int sched_getparam(pid_t pid, struct sched_param *param)
{
	int ret;

	ret = syscall_sched_get(pid, param, ID_PROCESS);
	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	return ret;
}

/*
 * sched_getscheduler - get scheduling policy (REALTIME)
 */
int sched_getscheduler(pid_t pid)
{
	int ret;

	ret = syscall_sched_get(pid, NULL, ID_PROCESS);
	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	return ret;
}

/*
 * sched_setparam - set scheduling parameters (REALTIME)
 */
int sched_setparam(pid_t pid, const struct sched_param *param)
{
	int ret;

	ret = syscall_sched_set(pid, 0, param, ID_PROCESS);

	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	return ret;
}

/*
 * sched_yield - yield the processor
 */
int sched_yield(void)
{
	return syscall_sched_yield();
}

/*
 * sched_setscheduler - set scheduling policy and parameters (REALTIME)
 */
int sched_setscheduler(pid_t pid, int policy,
       const struct sched_param *param)
{
	int ret;

	ret = syscall_sched_set(pid, policy, param, ID_PROCESS);

	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	return ret;
}
