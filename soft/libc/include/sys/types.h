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
 * File: sys/types.h
 *
 * The header is as defined by POSIX.
 * Contains basic data type declarations.
 */

#ifndef __SYS_TYPES_H
#define __SYS_TYPES_H

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

typedef unsigned long clockid_t;

/* for size of an object */
#ifndef __SIZE_T
#define __SIZE_T
typedef unsigned int size_t;
#endif

/* for pid values */
#ifndef __PID_T
#define __PID_T
typedef int pid_t;
#endif

/* for tid values */
#ifndef __TID_T
#define __TID_T
typedef int tid_t;
#endif

/* for user id values */
#ifndef __UID_T
#define __UID_T
typedef int uid_t;
#endif

/* for group id values */
#ifndef __GID_T
#define __GID_T
typedef int gid_t;
#endif

#ifndef NULL
#define NULL ((void *) 0)
#endif

typedef int ssize_t;


/* Used for timer ID returned by timer_create(). */
typedef int timer_t;

/* Used for some file attributes.  */
typedef int mode_t;	/* Opened file mode */

/* Used for file sizes. */
typedef unsigned long off_t;

/* Used for file serial numbers. */
typedef int ino_t;

typedef int blksize_t;

typedef unsigned long blkcnt_t;

/* Used for device id */
typedef unsigned long dev_t;

typedef int nlink_t;

typedef unsigned long useconds_t;

typedef long suseconds_t;	/* Used for time in microseconds.  */

/* Posix thread identifier */
typedef tid_t pthread_t;

struct sched_param{
	int sched_priority;
};

/* Posix thread attribute  */
struct pthread_attr{
	int flags;
	struct sched_param prio;
	void *stackaddr;
	size_t	stack_size;
	int policy;
};

typedef struct pthread_attr	pthread_attr_t;

#ifndef __KERNEL_LOCK_H
 #include <mrx/lock.h>
#endif /* __KERNEL_LOCK_H */

/* Pthread spinlock */
typedef spinlock_t pthread_spinlock_t;

/* Pthread Mutex attributes */
typedef mrx_mutexattr_t pthread_mutexattr_t;

/* Pthread mutex object */
typedef mrx_mutex_t pthread_mutex_t;

/* Pthread barrier object */
typedef mrx_barrier_t pthread_barrier_t;

/* Pthread Barrier attributes */
typedef mrx_barrierattr_t pthread_barrierattr_t;

/* Pthread Condition Variable attributes */
typedef mrx_condattr_t pthread_condattr_t;

/* Pthread Condition Variable Object */
typedef mrx_cond_t pthread_cond_t;

/* Pthread Reader Writer Lock attributes */
typedef mrx_rwlockattr_t pthread_rwlockattr_t;

/* Pthread Reader Writer Lock Object */
typedef mrx_rwlock_t pthread_rwlock_t;


typedef int pthread_key_t;

typedef int pthread_once_t;



#endif /* __SYS_TYPES_H */

