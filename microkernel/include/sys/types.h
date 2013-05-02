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
 * File: sys/types.h
 *
 * The header is as defined on Common UNIX specification.
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

typedef int ssize_t;
typedef unsigned int dev_t;
typedef unsigned int ino_t;
typedef unsigned long off_t;
typedef int mode_t;


#endif /* __SYS_TYPES_H */

