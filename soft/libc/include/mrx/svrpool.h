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
 * File: svrpool.h
 */

#ifndef __SVR_POOL_H
#define __SVR_POOL_H

#include <asm/ipc.h>
#include <asm/bit.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <sched.h>

#define POOL_INIT	0
#define POOL_DONE	1
#define POOL_BUSY	2

typedef void *(* pthtype_t)(void *);

struct poolstruct {
	int flag;
	pthread_t owner_id;	/* Which thread is servicing */
	pthread_t reply;	/* reply to this thread */
	int request;
	struct message msg;
	char data[MAX_MSG_LONG];
};

typedef struct poolstruct *poolstruct_t;

struct thrpool{
	poolstruct_t pool;
	void *(* altworker)(void *);	/* Alternative worker. */
	volatile unsigned int pool_bitmap; /* Bitmap to find out free and used threads */
	/* Lock for POOL */
	pthread_mutex_t poollock;
};

extern int no_func();
extern int  pool_init(struct thrpool *thrpool, int nthread,
	       void* (* worker)(void *));
extern void msg_loop(struct thrpool *thrpool, int sig);

#endif /* __SVR_POOL_H */
