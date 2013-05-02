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

#include <stdio.h>
#include <string.h>
#include <mrx/svrpool.h>

int no_func()
{
	return 0;
}

int  pool_init(struct thrpool *thrpool, int nthread,
	void* (* worker)(void *))
{
	pthread_attr_t attr;
	struct sched_param parm;
	poolstruct_t thr;
	size_t size;
	int ret, i;

	size = sizeof(struct poolstruct) * nthread;
	thrpool->pool = (poolstruct_t)malloc(size);

	if(thrpool->pool == NULL)
		return -1;
	memset(thrpool->pool, 0, size);
	pthread_attr_init(&attr);
	parm.sched_priority = PRIO_SYS_DEF;
	pthread_attr_setschedparam(&attr, &parm);

	pthread_mutex_init(&thrpool->poollock, NULL);

	for(i = 0; i < nthread; i++)
	{
		thr = &thrpool->pool[i];
		thr->flag = POOL_INIT;
		ret = pthread_create(&thr->owner_id, &attr, worker, thr);
//		if(ret != 0)
//			printf("%d'th thread creation failed with errorcode %d tid %d \n", i, ret, thr->owner_id);
	}

	return 0;
}

void msg_loop(struct thrpool *thrpool, int sig)
{
	poolstruct_t thr;
	int bitpos;

	sched_yield();

	while(1)
	{

		bitpos = ffc(thrpool->pool_bitmap);
		if((bitpos < 0) || (bitpos > 31))
		{
			if(thrpool->altworker)
				thrpool->altworker(NULL);
			continue;
		}
		set_bit(&thrpool->pool_bitmap, bitpos);

		thr = &thrpool->pool[bitpos];


		thr->msg.snd_ptr = &thr->data;
		thr->msg.rcv_ptr = &thr->data;
		
		/* wait till the thread in the pool is ready to serve */
/*		do{
			if(thr->flag & POOL_DONE)
				break;
			sched_yield();
		}while(1);
*/
		receive_message(0, &thr->msg);

		thr->flag |= POOL_BUSY;
//		if(thr->msg.flags & MSG_LONG_RECEIVE)
//			thr->msg.rcv_ptr = &thr->data;
		thr->request = thr->msg.w1;

		pthread_kill(thr->owner_id, sig);
	}
}
