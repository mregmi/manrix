/*
 * ManRiX Microkernel
 * Copyright (C) 2004, 2005
 * 	Manish Regmi (regmi dot manish at gmail dot com) and
 * 	Rajesh R.C   (rajee5283@hotmail.com)
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
 * File: sync.h
 *		Synchronization objects.
 */

#ifndef __KERNEL_SYNC_H
#define __KERNEL_SYNC_H

#include <kernel/list.h>
#include <sys/types.h>

#define sync_fn(owner, id)	(owner * id)

struct syncstruct{
	struct linked_list syncs;
	long id;
};

struct sync{
	int sync_type;
	int flags;
	int sync_id;
	struct linked_list wq;

	pid_t owner;
	struct sync *next;	/* Hash Link */
	struct linked_list syncs;	/* Syncs on same process */
};

typedef struct{
	struct sync *sync;
} *synchash_t;

extern struct sync *find_sync(pid_t , int );
extern void sync_init();
extern void delete_sync(struct syncstruct *);


#endif /* __KERNEL_SYNC_H */
