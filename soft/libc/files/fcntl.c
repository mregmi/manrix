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
 * File: fcntl.c
 */

#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#include <asm/ipc.h>
#include <mrx/fd.h>
#include <mrx/conmngr.h>


/*
 * open - open a file
 */
int open(const char *path, int oflag, ... )
{
	struct message msg;
	int length, ret,server;

	length = strlen(path);
	if(length > MAX_MSG_LONG)
		msg.flags = MSG_MAP_SEND;
	else
		msg.flags = MSG_LONG_SEND;

	msg.snd_ptr = (void *)path;
	msg.snd_bufsize = length+1;

	msg.w1 = FS_OPEN;
	msg.w4 = oflag;
	send_message(ID_FSIO_MNGR, &msg);

	ret = msg.w1;
	if(msg.w1 < 0)
	{
		errno = -msg.w1;
		return -1;
	}
	if(S_ISREG(msg.w4) || S_ISDIR(msg.w4))
		goto out;

	msg.flags = MSG_SHORT|MSG_NOREPLY;
	msg.w1 = SVR_OPEN;
	if((ret >= 0) && (ret < 3))
		server = ID_CON_MNGR;
	else
		server = FDESC_SVR(ret);
	send_message(server, &msg);

out:
	return ret;
}




