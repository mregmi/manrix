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
 * File:- ipc.c
 *	IPC related calls.
 */

#include <errno.h>
#include <mrx/mutex.h>

#include <asm/ipc.h>
#include <asm/syscall.h>

int send_message(tid_t tid, message_t msg)
{
	int ret;

	ret = syscall_send_message(tid, msg);
	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	return ret;
}

int receive_message(tid_t tid, message_t msg)
{
	int ret;

	ret = syscall_receive_message(tid, msg);

	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	return ret;

}

int reply_message(tid_t tid, message_t msg)
{
	int ret;
	ret = syscall_reply_message(tid, msg);
	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	return ret;

}



/*
 * Helper routine for mutex_lock
 * Mutex locked, sleep till the locker wakes us.
 */
void mrx_mutex_sleep(mrx_mutex_t *mt)
{
	syscall_sync_lock(mt->sync_id);
}

/*
 * Helper routine for mutex_unlock
 * Unlock the mutex and wake the sleepers.
 */
void mrx_mutex_wake(mrx_mutex_t *mt)
{
	syscall_sync_unlock(mt->sync_id);
}

