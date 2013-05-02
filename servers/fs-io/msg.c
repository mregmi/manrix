/*
 * FS and I/O manager for XXX OS
 * Copyright (C) 2004, 2005	Manish Regmi (regmi dot manish at gmail dot com)
 *				            Rajesh R.C (rajee5283 at hotmail dot com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/*
 * File: msg.c
 *
 * This File handles the sent messages and replies the sender.
 */

#include <asm/ipc.h>

#include "vfs.h"


/* open a file */
void msg_open(message_t msg)
{
	int ret;

	ret = vfs_open(msg->tid, (char *)msg->snd_ptr, (int *)&msg->w4, 0);
	msg->w1 = ret;
//	printf("reply %d ", msg->tid);
//	for(;;);
	reply_message(msg->tid, msg);
}

/* close the file */
void msg_close(message_t msg)
{
	int ret;

	ret = vfs_close(msg->tid, msg->w4);
	msg->w1 = ret;
	reply_message(msg->tid, msg);

}

void msg_read(message_t msg)
{
	int ret;

	ret = vfs_read(msg->tid, msg->w2, msg->rcv_ptr, msg->rcv_bufsize);
	msg->w1 = ret;
	reply_message(msg->tid, msg);
}

void msg_write(message_t msg)
{
	int ret;

	ret = vfs_write(msg->tid, msg->w4, msg->snd_ptr, msg->snd_bufsize);
	msg->w1 = ret;
	reply_message(msg->tid, msg);
}

/* determine accessibility of a file */
void msg_access(message_t msg)
{
	int ret;

	ret = vfs_access(msg->tid, msg->snd_ptr, msg->w4);
	msg->w1 = ret;
	reply_message(msg->tid, msg);
}

void msg_chdir(message_t msg)
{
	msg->w1 = fs_chdir(msg->tid, msg->snd_ptr);
	reply_message(msg->tid, msg);
}

/* Chown */
void msg_chown(message_t msg)
{
	msg->w1 = vfs_chown(msg->tid, msg->snd_ptr, msg->w4, msg->w5);
	reply_message(msg->tid, msg);
}

/*
 * Return value is kept in the same buf.
 */
void msg_exec(message_t msg)
{
	/*
	 * Return value is kept in the same buf.
	 */
	msg->w1 = do_exec(msg->tid, (char *)msg->snd_ptr, (struct vn_object *)msg->rcv_ptr);
	reply_message(msg->tid, msg);
}

/* Chmod */
void msg_chmod(message_t msg)
{
	msg->w1 = vfs_chmod(msg->tid, msg->snd_ptr, msg->w4);
	reply_message(msg->tid, msg);
}

/* stat */
void msg_stat(message_t msg)
{
	msg->w1 = vfs_chmod(msg->tid, msg->snd_ptr, msg->rcv_ptr);
	reply_message(msg->tid, msg);
}

/* fstat */
void msg_fstat(message_t msg)
{
	msg->w1 = vfs_fstat(msg->tid, msg->w2, msg->rcv_ptr);
	reply_message(msg->tid, msg);
}

void msg_data_req(message_t msg)
{
	msg->w1 = data_request(msg->tid, msg->snd_ptr, msg->rcv_ptr);
	reply_message(msg->tid, msg);
}

#ifdef SEPERATE_IO_PROCESS

#include <mrx/fd.h>
#include "chrdev.h"
#include "blockdev.h"

extern struct char_device chrdev[MAX_MAJOR];
extern struct bdevsw	bdevs[MAX_MAJOR];
#endif

void msg_req_iospace(message_t msg)
{
#ifdef SEPERATE_IO_PROCESS
	printf("werr \n");
	if(msg->w4 == DEV_CHAR)
	{
		chrdev[msg->w2].server = msg->w3;
	}
	else if(msg->w4 == DEV_BLK)
	{
		printf("Svr value %d written in %d\n",msg->w3, msg->w2);
		bdevs[msg->w2].server_id = msg->w3;
	}
#endif
}

