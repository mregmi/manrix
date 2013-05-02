/*
 * FS and I/O manager for ManRiX OS
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
 * File: chrdev.c
 *
 * The generic interface for Character devices.
 */
#include <string.h> 

#include "chrdev.h"
#include "vfs.h"

struct char_device chrdev[MAX_MAJOR];

#ifdef SEPERATE_IO_PROCESS
 int chrdev_register(int major, tid_t server_id)
#else
 int chrdev_register(int major, struct chevsw *chops)
#endif
{
	if((major >= MAX_MAJOR) || (major < 0))
		return -1;

#ifdef SEPERATE_IO_PROCESS
	chrdev[major].server = server_id;
#else
	chrdev[major].ops = chops;
#endif
	return 0;
}

#ifndef SEPERATE_IO_PROCESS
int chrdev_unregister(int major)
{
	if((major >= MAX_MAJOR) || (major < 0))
		return -1;

	chrdev[major].ops = NULL;

	return 0;
}
#endif

int chrdev_open(struct file *file, struct vnode *vn, mode_t mode)
{
	struct chevsw *chops ;
	
#ifdef SEPERATE_IO_PROCESS
	return chrdev[MAJOR(vn->v_rdev)].server;
#else
	chops = chrdev[MAJOR(vn->v_rdev)].ops;
	if(!chops || !chops->d_open)
		return 0;

	chops->d_open(file, vn, mode);
	return 0;
#endif
}

int chrdev_close(struct file *file, struct vnode *vn)
{
	if(!vn || !vn->ops || !vn->ops->vop_close)
		return 0;

#ifndef SEPERATE_IO_PROCESS
	chrdev[MAJOR(vn->v_rdev)].ops->d_close(file, vn);
#endif
	return 0;
}

#ifndef SEPERATE_IO_PROCESS
int chrdev_read(struct file *file, struct vnode *vn, void *data, size_t size)
{
	//if(!vn || !vn->ops || !vn->ops->vop_read)
	//	return 0;

	chrdev[MAJOR(vn->v_rdev)].ops->d_read(file, vn, data, size);
	return 0;
}

int chrdev_write(struct file *file, struct vnode *vn, void *data, size_t size)
{
	//if(!vn || !vn->ops || !vn->ops->write)
	//	return 0;

	chrdev[MAJOR(vn->v_rdev)].ops->d_write(file, vn, data, size);
	return 0;
}
#endif

struct vnodeops	chrops = {
	.vop_open = chrdev_open,
	.vop_close = chrdev_close,
	.vop_read = chrdev_read,
	.vop_write = chrdev_write
};


void chr_init()
{
	memset((void *)chrdev, 0, sizeof(struct char_device) * MAX_MAJOR);
}

void chr_exit()
{
}


/*#ifdef SEPERATE_IO_PROCESS
#include <asm/ipc.h>
int notify_blk_server(tid_t server, struct operation *op)
{
	struct message msg;

	msg.flags = MSG_LONG_SEND|MSG_MAP_RECIEVE;
	msg->w1 = ;
	msg.snd_ptr = op;
	msg.snd_bufsize = sizeof(struct operation);
	msg.rcv_ptr = op->buffer;
	msg.rcv_bufsize = op->nsects;

	return message_send(server, &msg);
}
#endif
*/
