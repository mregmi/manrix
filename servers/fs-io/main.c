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
 * File: main.c
 *
 * This is the main file of FS and I/O manager . They are implemented as single
 * process.
 */

#include <signal.h>
#include <pthread.h>
#include <stdio.h>
#include <stdarg.h>

#include <asm/ipc.h>
#include <mrx/fscases.h>
#include <mrx/svrpool.h>

#include "blkdev.h"
#include "msg.h"

#define MAX_FS_POOL		5
#define FS_REQUEST		SIGRTMIN

struct thrpool thrpool = {0};

typedef void (* fptr_t)(message_t );

fptr_t fs_work[] = {
	msg_data_req,
	msg_open,
	msg_close,
	msg_read,
	msg_write,
	msg_chown,
	msg_exec,
	msg_req_iospace,
	msg_access,
	msg_chdir,
	msg_chmod,
	msg_stat,
	msg_fstat,0

};


void fs_worker(poolstruct_t pool)
{
	sigset_t set;
	int num, off;
	fptr_t ptr;
	struct sigaction action;

	syscall_change_iopl(3);
	sigemptyset(&set);
	sigaddset(&set, FS_REQUEST);
	pthread_sigmask(SIG_SETMASK, &set, NULL);

	action.sa_handler = SIG_IGN;
	sigaction(FS_REQUEST, &action, NULL);
	pool->flag |= POOL_DONE;
//	printf("SIO Pool done ");

	off = ((int)pool - (int)thrpool.pool)/sizeof(struct poolstruct);

	while(1)
	{
		sigwait(&set, &num);
//		if(num != FS_REQUEST)
//			continue;

		ptr = fs_work[pool->request];
		ptr(&pool->msg);
		clear_bit(&thrpool.pool_bitmap, off);
	}
}

/* Alternative worker */
void *worker(poolstruct_t pool)
{
	fptr_t ptr;
	syscall_change_iopl(3);
	ptr = fs_work[pool->request];
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
	pthread_create(&thr->owner_id, &attr, (pthtype_t)worker, thr);
}

/* this thread initializes the VFS */
/*void vfs_thread()
{
	dev_t dev;
	
	dev = DEVICE(1, 0, 6);
	vfs_init("ext2", dev);
}
*/

/* this for a short period only; till 
 * libc contains strtol()
 */ 
int simple_atoi(char *str)
{
	int num = 6;
	
	if(str[0] != '\0')
		num = str[0] - '0';
	
	return num;
}


/*
 * The main thread for the FS/IO manager.
 * we create a seperate thread for i/o and handle it in seperate file.
 * We handle fs manager here. we create a pool of threads and they
 * handle the request.
 */
int main(int argc, char *argv[])
{
	dev_t dev;
//	char *root;
//	pthread_attr_t attr;
	int num  = 7, ret;
	
	printf("\nFSIO manager Initializing.... ");
	syscall_change_iopl(3);

	io_main();
	
	if((argv[1] != NULL) && (argc > 1))
		num = simple_atoi(argv[1]);
	
	printf("disk is %d argc %d ", num, argc);
	dev = DEVICE(1, 0, num);
	vfs_init("ext2", dev);

	//	pthread_attr_init(&attr);
//	pthread_create(NULL, &attr, vfs_thread, NULL);

	thrpool.altworker = (pthtype_t)handle_full;
	thrpool.pool_bitmap = 0;
	ret = pool_init(&thrpool, MAX_FS_POOL, (pthtype_t)fs_worker);

	printf("FSIO Done  ");
	msg_loop(&thrpool, FS_REQUEST);

	return 0;
}
