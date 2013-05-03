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
 *
 *
 * File: chrdev.h
 */

#ifndef __CHRDEV_H
#define __CHRDEV_H

#include <sys/types.h>

#include "vfs.h"

struct chevsw{
	int (* d_open)(struct file *, struct vnode *, mode_t);
	int (* d_close)(struct file *, struct vnode *);
	int (* d_read)(struct file *, struct vnode *, void *, size_t );
	int (* d_write)(struct file *, struct vnode *, void *, size_t);
	int (* d_ioctl)();
	int (* d_mmap)();
};

struct char_device {
	struct chevsw *ops;
	tid_t server;
};

extern int chrdev_register(int, struct chevsw *);

extern int chrdev_unregister(int );
extern int chrdev_open(struct file *, struct vnode *, mode_t);
extern int chrdev_close(struct file *, struct vnode *);
extern int chrdev_read(struct file *, struct vnode *, void *, size_t );
extern int chrdev_write(struct file *, struct vnode *, void *, size_t );

extern void chr_init();
extern void chr_exit();

#endif	/* __CHRDEV_H */
