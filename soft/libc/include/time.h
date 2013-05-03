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


#ifndef __TIME_H
#define __TIME_H

#ifndef __SYS_TYPES_H
#include <sys/types.h>
#endif

#ifndef NULL
#define NULL    ((void *)0)
#endif

#define CLOCK_TICK		  1000000
#define CLOCK_PER_SECOND  CLOCK_TICK


#define CLOCK_REALTIME		1
#define CLOCK_ABSTIME		0


extern int    daylight;
extern long   timezone;
extern char  *tzname[];


/* Used by other time functions.  */
struct tm
{
  int tm_sec;			/* Seconds.	[0-60] (1 leap second) */
  int tm_min;			/* Minutes.	[0-59] */
  int tm_hour;			/* Hours.	[0-23] */
  int tm_mday;			/* Day.		[1-31] */
  int tm_mon;			/* Month.	[0-11] */
  int tm_year;			/* Year	- 1900.  */
  int tm_wday;			/* Day of week.	[0-6] */
  int tm_yday;			/* Days in year.[0-365]	*/
  int tm_isdst;			/* DST.		[-1/0/1]*/
};


struct timespec{
time_t  tv_sec;         /* Seconds.     */
long    tv_nsec;        /* Nanoseconds. */
};


struct itimerspec
  {
    struct timespec it_interval;		/* Timer period      */
    struct timespec it_value;			/* Timer Expiration  */
  };

#ifndef __SIGNAL_H
 #include <signal.h>
#endif

/* get the cpu time used */
extern clock_t    clock(void);

/*
 * gets the resolutions for any clock
 */
extern int clock_getres(clockid_t, struct timespec *);

/* gets the current value of timespec structure for the specified clock,clock_id*/
extern int  clock_gettime(clockid_t, struct timespec *);

/* set the specified clock,clock id,to the value specified by structure*/
extern int  clock_settime(clockid_t, const struct timespec *);

/* high resolution sleep with specified clock (Advanced Realtime) */
extern int  clock_nanosleep(clockid_t, int, const struct timespec *,struct timespec *);

extern char *ctime(const time_t *);

extern double difftime(time_t, time_t);

extern struct tm *getdate(const char *);

extern struct tm *gmtime(const time_t *);

extern struct tm *gmtime_r(const time_t *__restrict, struct tm *__restrict);

extern struct tm *localtime(const time_t *);

extern struct tm *localtime_r(const time_t *__restrict, struct tm *__restrict);

/*
 * convert broken-down time into time since the Epoch
 */
extern time_t mktime(struct tm *);

extern int    nanosleep(const struct timespec *, struct timespec *);

extern size_t strftime(char *__restrict, size_t, const char *__restrict,\
                       const struct tm *__restrict);
extern char      *strptime(const char *__restrict, const char *__restrict,\
                   struct tm *__restrict);
/*
 * get time
 */
extern time_t time(time_t *);

/*
 * timer_create - create a per-process timer (REALTIME)
 */
 extern int timer_create(clockid_t, struct sigevent *, timer_t *);

/*
 * delete a per-process timer (REALTIME)
 */
 extern int timer_delete(timer_t);

/*
 * store the amount of time until the specified timer expires
 * and the reload value of the timer into the space pointed to by the
 * value argument.
 */
extern int timer_gettime(timer_t, struct itimerspec *);

extern int timer_settime(timer_t, int, const struct itimerspec *__restrict,
                  struct itimerspec *__restrict);


#endif /*_TIME_H */
