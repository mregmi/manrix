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
/*	file: process.c
 *
 */
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <asm/ipc.h>
#include <asm/asm.h>
#include <asm/syscall.h>
#include <mrx/pthread-internal.h>


/*
 * fork - create a new process
 */
pid_t fork(void)
{
	pid_t pid;
	struct pthread_internal *pth;

	/* Execute Fork handlers */
	pth = get_pinternal();
	if(pth->pt_fork.prepare)
		pth->pt_fork.prepare();

	pid = syscall_fork();
	if(pid < 0)
	{
		errno = -pid;
		return -1;
	}
	else if(pid > 0)
	{
//		if(pth->pt_fork.parent)
//			pth->pt_fork.parent();

		return pid;
	}

//	if(pth->pt_fork.child)
//		pth->pt_fork.child();
	return pid;
}

/*
 * vfork - create a new process; share virtual memory
 */
/* 
pid_t vfork(void)
{
	pid_t pid;
	struct pthread_internal *pth;

	/* Execute Fork handlers */
//	pth = get_pinternal();
//	if(pth->pt_fork.prepare)
//		pth->pt_fork.prepare();

/*	pid = syscall_vfork();
	if(pid < 0)
	{
		errno = -pid;
		return -1;
	}
	else if(pid > 0)
	{
//		if(pth->pt_fork.parent)
//			pth->pt_fork.parent();

		return pid;
	}

//	if(pth->pt_fork.child)
//		pth->pt_fork.child();

	return pid;
}
*/
void _exit(int status)
{
	syscall_exit(status);
}

void exit(int status)
{
	syscall_exit(status);
}

pid_t wait(int *status)
{
	int ret;
	
	ret = syscall_wait(0, status, 0);
	if(ret < 0)
	{
		errno = -ret;
		ret = -1;
	}
	
	return ret;
}

