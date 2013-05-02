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
 * File:- time.c
 *		Time and Timer Implementation.
 */

#include <time.h>
#include <sys/times.h>
#include <signal.h>
#include <errno.h>



#include <asm/syscall.h>

/*
 * clock - report CPU time used
 */
clock_t clock()
{
	return syscall_clock(NULL);
}

 /*
  * clock_getres:- gives the resolution of any clock
  * we have implemented for realtime clock only
  */

int clock_getres(clockid_t clock, struct timespec *res)
 {

	int ret;

	if(clock != CLOCK_REALTIME)
	{
		errno = EINVAL;
		return -1;
	}

	ret = syscall_getresolution(clock, res);

	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	return 0;

}

/* gets the current value of tp for specified clock,clock_id*/
int  clock_gettime(clockid_t clock_id, struct timespec *tp)
{
	int ret;
	if(clock_id != CLOCK_REALTIME)
	{
		errno = EINVAL;
		return -1;
	}

	ret = syscall_gettime(clock_id, tp);
	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	return 0;
}

/* set the specified clock,clock id,to the value specified by tp*/
int  clock_settime(clockid_t clk, const struct timespec *tp)
{
	int ret;

	if(clk != CLOCK_REALTIME)
	{
		errno = EINVAL;
		return -1;
	}
	ret = syscall_settime(clk, tp);
	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	return 0;
}

struct tm *getdate(const char *string)
{
	//implementation after the implementation of file system //
	return NULL;
}


/*
 * convert broken-down time into time since the Epoch
 */
time_t mktime(struct tm *timeptr)
{

	long time;

	if (timeptr->tm_year < 69)
    return -1;

    if (timeptr->tm_sec < 0)
	    timeptr->tm_sec = 0;
	if (59 < timeptr->tm_sec)
    	timeptr->tm_sec = 59;

	time=timeptr->tm_sec + timeptr->tm_min*60 +     \
		timeptr->tm_hour*3600 + 					\
		timeptr->tm_yday*86400 +					\
    	(timeptr->tm_year-70)*31536000 +            \
    	((timeptr->tm_year-69)/4)*86400 -			\
    	((timeptr->tm_year-1)/100)*86400 +          \
    	((timeptr->tm_year+299)/400)*86400 ;

	if(time <= 0)
	{
		errno=EOVERFLOW;
		return (time_t)-1;

	}
	return time;
}

/*
 * get time
 */
time_t time(time_t *tloc)
{
	time_t tm;

	tm = syscall_gettime(CLOCK_REALTIME, NULL);
	if(tloc)
		*tloc = tm;

	return tm;
}

/*
 * create a per-process timer (REALTIME)
 */
int timer_create(clockid_t clockid, struct sigevent *evp, timer_t *timerid)
{
	int tmrid;
	int flag;

	if(clockid != CLOCK_REALTIME)
	{
		errno= EINVAL;
		return -1;
	}
	flag = TM_TIMER;
	tmrid = syscall_timer_create(clockid, flag, evp, timerid);

	if(tmrid < 0)
	{
		errno= -tmrid;
		return -1;
	}

	return 0;
}

 /*
  * delete a per-process timer (REALTIME)
  */
 int timer_delete(timer_t timerid)
 {
	 int ret;
	 if(timerid < 0)
	 	return -1;
	 ret=syscall_timer_delete(timerid);
	 if(ret <0)
	 {
	 	errno= - ret;
	 	return -1;
	 }
	 return 0;

}

int timer_gettime(timer_t timerid, struct itimerspec *value)
{
	int ret;
	ret= syscall_timer_gettime(timerid,value);
	if(ret < 0)
	{
		errno=-ret;
		return -1;
	}

	return 0;
}

int nanosleep(const struct timespec *rqtp, struct timespec *rmtp)
{
	int ret;


	if( (!rqtp) || (rqtp->tv_nsec < 0) || (rqtp->tv_nsec > 10000000))
	{
		errno = EINVAL;
		return -1;
	}

	ret  = syscall_nanosleep(rqtp, rmtp);
	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	return ret;
}
