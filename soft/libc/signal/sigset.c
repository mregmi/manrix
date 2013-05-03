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
 * File:- Sigset.c
 *	Signal set implkementations.
 */

#include <signal.h>
#include <errno.h>
#include <limits.h>

#include <asm/bit.h>

/*
 * sigemptyset - initialize and empty a signal set
 */
int sigemptyset(sigset_t *set)
{
	if(!set)
	{
		errno = EINVAL;
		return -1;
	}

	*set = 0;

	return 0;
}

/*
 * sigaddset - add a signal to a signal set
 */
int sigaddset(sigset_t *set, int signo)
{
	if((signo < 0) || (signo > MAX_SIGNAL) || (!set))
	{
		errno = EINVAL;
		return -1;
	}

	set_bit(set, signo);

	return 0;
}

/*
 * sigdelset - delete a signal from a signal set
 */
int sigdelset(sigset_t *set, int signo)
{
	if((signo < 0) || (signo > MAX_SIGNAL) || (!set))
	{
		errno = EINVAL;
		return -1;
	}

	clear_bit(set, signo);

	return 0;
}

/*
 * sigfillset - initialize and fill a signal set
 */
int sigfillset(sigset_t *set)
{
	if(!set)
	{
		errno = EINVAL;
		return -1;
	}

	*set = UINT_MAX;

	return 0;
}

/*
 * sigismember - test for a signal in a signal set
 */
int sigismember(const sigset_t *set, int signo)
{
	if((signo < 0) || (signo > MAX_SIGNAL) || (!set))
	{
		errno = EINVAL;
		return -1;
	}

	if(check_set(*set, signo))
		return 1;
	else
		return 0;
}

