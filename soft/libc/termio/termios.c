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
 * termios.c
 * 	termios  handling routines.
 */

#include <termios.h>
#include <errno.h>

#include <asm/ipc.h>
#include <mrx/fd.h>
#include <mrx/conmngr.h>

/*
 * tcgetattr - get the parameters associated with the terminal
 */
int tcgetattr(int fildes, struct termios *termios_p)
{
	struct message msg;

	msg.flags = MSG_MAP_RECEIVE;
	msg.rcv_ptr = termios_p;
	msg.rcv_bufsize = sizeof(struct termios);
	msg.w1 = CON_GETATTR;
	msg.w4 = FDESC_FD(fildes);

	send_message(ID_CON_MNGR, &msg);

	if(msg.w1 < 0)
	{
		errno = -msg.w1;
		return -1;
	}

	return 0;
}

/*
 * tcsetattr - set the parameters associated with the terminal
 */
int tcsetattr(int fildes, int optional_actions,
       const struct termios *termios_p)
{
	struct message msg;

	msg.flags = MSG_LONG_SEND;
	msg.snd_ptr = (void *)termios_p;
	msg.snd_bufsize = sizeof(struct termios);
	msg.w1 = CON_SETATTR;
	msg.w4 = FDESC_FD(fildes);

	send_message(ID_CON_MNGR, &msg);

	if(msg.w1 < 0)
	{
		errno = -msg.w1;
		return -1;
	}

	return 0;
}

/*
 * tcgetsid - get the process group ID for the session leader
 * for the controlling terminal
 */
pid_t tcgetsid(int fildes)
{
	struct message msg;

	msg.flags = MSG_SHORT;
	msg.w1 = CON_GETSID;
	msg.w4 = FDESC_FD(fildes);

	send_message(ID_CON_MNGR, &msg);

	if(msg.w1 < 0)
	{
		errno = -msg.w1;
		return -1;
	}

	return msg.w1;
}

/*
 * tcflush - flush non-transmitted output data,
 * non-read input data, or both
 */
int tcflush(int fildes, int queue_selector)
{
	struct message msg;

	msg.flags = MSG_SHORT;
	msg.w1 = CON_TCFLUSH;
	msg.w4 = FDESC_FD(fildes);
	msg.w2 = queue_selector;

	send_message(ID_CON_MNGR, &msg);

	if(msg.w1 < 0)
	{
		errno = -msg.w1;
		return -1;
	}

	return 0;
}

