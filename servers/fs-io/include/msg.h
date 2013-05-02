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
#ifndef __FSIO_MSG_H
#define __FSIO_MSG_H

#include <asm/ipc.h>

#include "vfs.h"


/* open a file */
extern void msg_open(message_t msg);

extern void msg_read(message_t msg);
extern void msg_write(message_t msg);

/* close the file */
extern void msg_close(message_t msg);

/* determine accessibility of a file */
extern void msg_access(message_t msg);

extern void msg_chdir(message_t msg);

/* Chown */
extern void msg_chown(message_t msg);

extern void msg_exec(message_t msg);

extern void msg_data_req(message_t msg);

extern void msg_req_iospace(message_t msg);
extern void msg_chmod(message_t msg);
extern void msg_stat(message_t msg);
extern void msg_fstat(message_t msg);

#endif /* __FSIO_MSG_H */
