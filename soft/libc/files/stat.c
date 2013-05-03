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
 * File:- stat.c
 */

#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <asm/ipc.h>

/* chmod - change mode of a file */
int  chmod(const char *path, mode_t mode)
{
	struct message msg;
	int length;

	length = strlen(path);
	if(length > MAX_MSG_LONG)
		msg.flags = MSG_MAP_SEND;
	else
		msg.flags = MSG_LONG_SEND;

	msg.snd_ptr = (void *)path;
	msg.snd_bufsize = length;

	msg.w4 = mode;
	msg.w1 = FS_CHMOD;
	send_message(ID_FSIO_MNGR, &msg);

	if(msg.w1 < 0)
	{
		errno = -msg.w1;
		return -1;
	}

	return 0;
}

/*
 * stat - get file status
 */
int stat(const char *__restrict path, struct stat *__restrict buf)
{
	struct message msg;
	int length;

	length = strlen(path);
	if(length > MAX_MSG_LONG)
		msg.flags = MSG_MAP_SEND;
	else
		msg.flags = MSG_LONG_SEND;

	msg.snd_ptr = (void *)path;
	msg.snd_bufsize = length + 1;
	msg.rcv_ptr = buf;
	msg.rcv_bufsize = sizeof(struct stat);
	msg.flags |= MSG_LONG_RECEIVE;

	msg.w1 = FS_STAT;
	send_message(ID_FSIO_MNGR, &msg);

	if(msg.w1 < 0)
	{
		errno = -msg.w1;
		return -1;
	}

	return 0;
}

/*
 * fstat - get file status
 */ 
 int fstat(int fildes, struct stat *buf)
{
	struct message msg;
	
	msg.flags = MSG_LONG_RECEIVE;
	msg.rcv_ptr = buf;
	msg.rcv_bufsize = sizeof(struct stat);

	msg.w1 = FS_FSTAT;
	send_message(ID_FSIO_MNGR, &msg);

	if(msg.w1 < 0)
	{
		errno = -msg.w1;
		return -1;
	}

	return 0;
}
	 
