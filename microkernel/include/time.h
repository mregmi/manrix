/*
 * ManRiX Microkernel
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
 * File: time.h
 *
 * The header is as defined on Common UNIX specification,
 * and ANSI C.
 */

#ifndef __TIME_H
#define __TIME_H

#ifndef NULL
#define NULL		((void *)0)
#endif	/* NULL */

/* Number of clock ticks per second. */
#define CLK_TCK				1000000
#define CLOCKS_PER_TICK		CLK_TCK

#define CLOCK_REALTIME		0
#define TIMER_ABSTIME		1	/* not implemented yet */

/* For storing the current time and date. It is the number of
 * seconds since midnight Jan 1, 1970.
 */
#ifndef __TIME_T
#define __TIME_T
typedef long time_t;
#endif	/* __TIME_T */

/*
 * For measuring processor time in clock ticks.
 */
#ifndef __CLOCK_T
#define __CLOCK_T
typedef	long	clock_t;
#endif /* __CLOCK_T */

typedef int clockid_t;

struct timespec{
	time_t	tv_sec;
	long	tv_nsec;
};

struct itimerspec{
	struct timespec  it_interval;
	struct timespec  it_value;
};

/* structure tms, which is returned by times() */
struct tms{
	clock_t tms_utime;
	clock_t tms_stime;
	clock_t tms_cutime;
	clock_t tms_cstime;
};

#endif  /* __TIME_H */
