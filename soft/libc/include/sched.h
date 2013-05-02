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

#ifndef __SCHED_H
#define __SCHED_H

#ifndef __SYS_TYPES_H
#include <sys/types.h>
#endif


/* Scheduling Policies supported by system */
#define SCHED_FIFO			1
#define SCHED_RR			2
#define SCHED_OTHER			4

/* Scheduling Policies supported by system */
#define SCHED_FIFO			1
#define SCHED_RR			2
#define SCHED_OTHER			4

/* Different class of priorities. */
#define PRIO_RT_MAX			127		/* For Real-Time systems. */
#define PRIO_RT_DEF			107
#define PRIO_RT_MIN			88

#define PRIO_SYS_MAX			87
#define PRIO_SYS_DEF			66		/* For System-programs. */
#define PRIO_SYS_MIN			46

#define PRIO_OTHER_MAX			45
#define PRIO_OTHER_DEF			24		/* For Others. */
#define PRIO_OTHER_MIN			4

#define PRIO_IDLE			0		/* for idle threads */

#define MIN_PRIO			PRIO_OTHER_MIN
#define MAX_PRIO			PRIO_RT_MAX
/*
struct sched_param {
	int sched_priority;
};
*/
/*
 * sched_get_priority_max - get priority limits (REALTIME)
 * For now it is static.
 */
int    sched_get_priority_max(int);

/*
 * sched_get_priority_max - get priority limits (REALTIME)
 * For now it is static.
 */
int    sched_get_priority_min(int);

/*
 * sched_getparam - get scheduling parameters (REALTIME)
 */
int    sched_getparam(pid_t, struct sched_param *);

#ifndef __TIME_H
 #include <time.h>
#endif

/*
 * sched_getscheduler - get scheduling policy (REALTIME)
 */
int    sched_getscheduler(pid_t);

int    sched_rr_get_interval(pid_t, struct timespec *);

/*
 * sched_setparam - set scheduling parameters (REALTIME)
 */
int    sched_setparam(pid_t, const struct sched_param *);

/*
 * sched_setscheduler - set scheduling policy and parameters (REALTIME)
 */
int    sched_setscheduler(pid_t, int, const struct sched_param *);

/*
 * sched_yield - yield the processor
 */
int    sched_yield(void);



#endif /* __SCHED_H */

