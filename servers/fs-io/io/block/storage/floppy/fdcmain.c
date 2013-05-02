/*
 * FDC Server for ManRiX OS
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
 * File: fdcmain.c
 *	 The Floppy Disk Controller (FDC) driver server implementation.
 */

#include "svrconfig.h"


#ifdef SEPERATE_IO_PROCESS

#include <mrx/blkdev.h>
#include <mrx/svrpool.h>
#include <mrx/fd.h>

#include <asm/ipc.h>

#include "fdc.h"

#define MAX_SVR_THREADS 	2
#define SVR_REQUEST			SIGRTMIN

struct thrpool svrpool = {0};
typedef int (* fptr_t)(message_t );

fptr_t svr_func[] = {
	no_func,
	svr_open,
	svr_close,
	svr_read,
	svr_write,
	svr_strategy,
	0
};

void svr_worker(poolstruct_t pool)
{
	sigset_t set;
	int num, off;
	fptr_t ptr;
	struct sigaction action;

	syscall_change_iopl(3);

	sigemptyset(&set);
	sigaddset(&set, SVR_REQUEST);
	pthread_sigmask(SIG_SETMASK, &set, NULL);

	action.sa_handler = SIG_IGN;
	sigaction(SVR_REQUEST, &action, NULL);
	pool->flag |= POOL_DONE;
	off = ((unsigned int)pool - (unsigned int)svrpool.pool)/sizeof(struct poolstruct);

	while(1)
	{
		sigwait(&set, &num);

		ptr = svr_func[pool->request];
		ptr(&pool->msg);
		clear_bit(&svrpool.pool_bitmap, off);
	}
}

/* Alternative worker */
void *worker(poolstruct_t pool)
{
	fptr_t ptr;
	syscall_change_iopl(3);
	ptr = svr_func[pool->request];
	ptr(&pool->msg);

	return NULL;
}

/*
 * This is an alternative message handler.
 * It is called only when all pools are used.
 */
void handle_full(void *ptr)
{
	pthread_attr_t attr;
	struct sched_param parm;
	poolstruct_t thr;

	thr = (poolstruct_t)malloc(sizeof(struct poolstruct));

	if(thr == NULL)
		return ;

	pthread_attr_init(&attr);
	parm.sched_priority = PRIO_SYS_DEF;
	pthread_attr_setschedparam(&attr, &parm);
	pthread_create(&thr->owner_id, &attr, worker, thr);
}


int main(int argc, char *argv[])
{
	printf("FDC: Initializing Floppy (FDC) driver. \n");
	syscall_change_iopl(3);
	fdc_init();

	svrpool.altworker = handle_full;
	svrpool.pool_bitmap = 0;
	pool_init(&svrpool, MAX_SVR_THREADS, svr_worker);
	msg_loop(&svrpool, SVR_REQUEST);

	return 0;
}

int svr_open(message_t msg)
{
	if(msg->flags & MSG_NOREPLY)
		return;

	reply_message(msg->tid, msg);
	return 0;
}

int svr_close(message_t msg)
{
	if(msg->flags & MSG_NOREPLY)
		return 0;

	reply_message(msg->tid, msg);
	return 0;
}

int svr_size(message_t msg)
{
	//return disk size;
	return 0;
}



int svr_read(message_t msg)
{
	struct operation *op;

	op = (struct operation *)msg->snd_ptr;
	op->caller = msg->tid;

	fdc_strategy(op);

	reply_message(msg->tid, msg);

	return 0;
}

int svr_write(message_t msg)
{
	struct operation *op;

	op = (struct operation *)msg->snd_ptr;
	op->caller = msg->tid;

	fdc_strategy(op);

	reply_message(msg->tid, msg);

	return 0;
}

int request_ionamespace()
{
	struct message msg;

	msg.flags = MSG_SHORT;
	msg.w4 = DEV_BLK;
	msg.w3 = pthread_self();
	msg.w2 = 1;	/* Major no for ATA */
	msg.w1 = GET_IOSPACE;
 	printf("Sending MSG to %d msg %d \n", ID_FSIO_MNGR, GET_IOSPACE);
	return send_message(ID_FSIO_MNGR, &msg);
}

#endif /* SEPERATE_IO_PROCESS */
