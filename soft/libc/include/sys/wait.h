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
 * File: sys/wait.h
 * 		File statistics.
 */

#ifndef __SYS_WAIT_H
#define __SYS_WAIT_H

#ifndef __SYS_TYPES_H
#include <sys/types.h>
#endif	/* __SYS_TYPES_H */

#ifndef __SIGNAL_H
 #include <signal.h>
#endif

#define WNOHANG			/* Do not hang if no status is available;
							return immediately.*/
#define WUNTRACED		/* Report status of stopped child process. */

/* macros for analysis of process status values */
#define WEXITSTATUS				/* Return exit status. */
#define WIFCONTINUED			/* True if child has been continued. */
#define WIFEXITED				/* True if child exited normally. */
#define WIFSIGNALED				/* True if child exited due to uncaught signal. */
#define WIFSTOPPED				/* True if child is currently stopped. */
#define WSTOPSIG				/* Return signal number that caused process to stop. */
#define WTERMSIG				/* Return signal number that caused process to terminate. */

/* possible values for the options argument to waitid(): */
#define WEXITED 				/* Wait for processes that have exited.  */
#define WSTOPPED 				/* Status is returned for any child that
									has stopped upon receipt of a signal.  */
#define WCONTINUED 				/* Status is returned for any child that
									was stopped and has been continued.  */
#define WNOHANG 				/* Return immediately if there are no children to wait for. */
#define WNOWAIT 				/* Keep the process whose status is returned in infop in a waitable state.  */


pid_t  wait(int *);
//int    waitid(idtype_t, id_t, siginfo_t *, int);
pid_t  waitpid(pid_t, int *, int);






#endif /* __SYS_WAIT_H */
