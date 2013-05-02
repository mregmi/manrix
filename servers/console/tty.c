/*
 * Console manager for ManRiX OS
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
 * File: tty.c
 * 	The generic interface for terminal devices.
 */

#include <errno.h>
#include <sched.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include <mrx/svrpool.h>
#include <mrx/fd.h>

#include <asm/syscall.h>

#include "keyboard.h"
#include "vga.h"
#include "tty.h"

#define MAX_TTY_THREADS		5
#define TTY_REQUEST		SIGRTMIN

struct thrpool ttypool = {0};
struct terminal *ttys = NULL;
int termindex = 0, ntty = 5;





/* Process Kill Character */
static inline void process_vkill(struct termios *trm, struct termqueue *q)
{
	while(q->end != 0)
	{
		if((q->queue[q->end] != trm->c_cc[VEOL]) ||
			(q->queue[q->end] != trm->c_cc[VEOF]) ||
			(q->queue[q->end] != SCODE_NL))
			break;
		q->end--;
	}
}

/* process erase character */
static inline void process_verase(struct termios *trm, struct termqueue *q)
{
	if((q->queue[q->end] != trm->c_cc[VEOL]) ||
		(q->queue[q->end] != trm->c_cc[VEOF]) ||
		(q->queue[q->end] != SCODE_NL))
		return;

	q->end = 0;
}

/* Read the character and put it in the input queue */
void read_key(unsigned char ch)
{
	struct termqueue *q = &TTY.t_input;
	struct termios *trm = &TTY.t_termios;

	if(trm->c_lflag & ISIG)
	{
		if(ch == trm->c_cc[VINTR])
			killpg(TTY.pgid, SIGINT);
		else if(ch == trm->c_cc[VQUIT])
			killpg(TTY.pgid, SIGQUIT);
		else if(ch == trm->c_cc[VSTART])
			;//	kill();
		else if(ch == trm->c_cc[VSTOP])
			;//	kill();
		else if(ch == trm->c_cc[VSUSP])
			killpg(TTY.pgid, SIGTSTP);
	}

	if(!(trm->c_lflag & ICANON))
		goto do_q;

	if(ch == trm->c_cc[VKILL])
	{
		process_vkill(trm, q);
		goto out;
	}

	else if(ch == trm->c_cc[VERASE])
	{
		process_verase(trm, q);
		goto out;
	}

do_q:
	if(ch == 0)
		goto out;


	if((ch == trm->c_cc[VEOL]) || (ch == trm->c_cc[VEOF]))
	{
		q->flag = FLAG_BREAK;
		goto wakenout;
	}

	else
		q->queue[q->end++] = ch;
wakenout:
	if(q->thread)
		syscall_wake_unint(q->thread);
	q->thread = 0;
out:
	return;
}

/* Read in Canonical Mode */
static inline int icanon_read(pthread_t tid, void *buf,
	size_t size, struct terminal *trm)
{
	int nread;
//	con_printf("ican reading %d ", size);

	while(trm->t_input.end < size)
	{
		con_printf("", trm->t_input.end);
		if(trm->t_input.flag == FLAG_BREAK)
		{
			trm->t_input.flag = 0;
			break;
		}
		trm->t_input.thread = pthread_self();
		syscall_sleep_unint(NULL);	/* Put to uninterruptible sleep */
	}

	memcpy(buf, &trm->t_input.queue, size);
	nread = trm->t_input.end;
	trm->t_input.end = 0;

	return nread;
}

/* Read in Non-Canonical Mode */
static inline int raw_read(pthread_t tid, void *buf,
	size_t size, struct terminal *trm)
{
	int nread;

//	scr_write("raw reading ");
	if((trm->t_termios.c_cc[VMIN] == 0) && (trm->t_termios.c_cc[VTIME] == 0))
	{
		memcpy(buf, &trm->t_input.queue, trm->t_input.end);
		nread = trm->t_input.end;
		trm->t_input.end = 0;
	}
again:
	if(trm->t_input.end < trm->t_termios.c_cc[VMIN])
	{
		syscall_sleep_unint(tid);	/* Put to uninterruptible sleep */
		goto again;
	}

	memcpy(buf, &trm->t_input.queue, size);
	return nread;
}

int msg_con_open(message_t msg)
{
	struct terminal *tm = &TTY;
	struct userid id;
//	scr_write("I am Opened ");
	syscall_get_user(msg->tid, &id);

	tm->pgid =  id.gid;

	if(msg->flags & MSG_NOREPLY)
		return 0;

	reply_message(msg->tid, msg);

	return 0;
}

int msg_con_close(message_t msg)
{
	struct terminal *tm = &TTY;
	struct userid id;

	syscall_get_user(msg->tid, &id);

	tm->pgid =  id.gid;

	if(msg->flags & MSG_NOREPLY)
		return 0;

	reply_message(msg->tid, msg);

	return 0;
}

/* Read Console */
int msg_con_read(message_t msg)
{
	struct terminal *tm = &TTY;
	void *buf = msg->rcv_ptr;
	size_t size = msg->rcv_bufsize;

//	con_printf("%d ", size);
	if(tm->pgid != getpgid(msg->tid))
		pthread_kill(msg->tid, SIGTTIN);

	if(tm->t_termios.c_lflag & ICANON)
		msg->w1 = icanon_read(msg->tid, buf, size, tm);
	else
		msg->w1 = raw_read(msg->tid, buf, size, tm);

	reply_message(msg->tid, msg);

	return msg->w1;
}

int msg_con_write(message_t msg)
{
	char *buf = (char *)msg->snd_ptr;

//	if(msg->tid >= 500)
//		kprintf("by %d ", msg->tid);
	scr_write(buf);

	if(msg->flags & MSG_NOREPLY)
		return 0;

	return 0;
}

/* Get the terminal attributes */
int msg_tcgetattr(message_t msg)
{
	struct termios *tm = msg->snd_ptr;
	struct terminal *term = &TTY;

	*tm = term->t_termios;

	reply_message(msg->tid, msg);

	return 0;
}

/* Set the terminal attributes */
int msg_tcsetattr(message_t msg)
{
	struct termios *tm = msg->snd_ptr;
	struct terminal *term = &TTY;

//	con_printf("terminal set %X ", tm->c_lflag);
	memcpy(&term->t_termios, tm, sizeof(struct termios));

	reply_message(msg->tid, msg);

	return 0;
}

/*
 * get the process group ID for the session leader
 * for the controlling terminal
 */
int msg_tcgetsid(message_t msg)
{
	struct terminal *term = &TTY;

	msg->w1 = term->pgid;
	reply_message(msg->tid, msg);

	return 0;
}

/*
 * flush non-transmitted output data,
 * non-read input data, or both
 */
int msg_tcflush(message_t msg)
{
	struct terminal *term = &TTY;
	struct termqueue *input;
//	struct termqueue *output;
	int type = msg->w2;

	if(type & TCIFLUSH)
	{
		input = &term->t_input;
		input->end = 0;
	}

	reply_message(msg->tid, msg);

	return 0;
}

typedef int (* fptr_t)(message_t );

fptr_t con_func[] = {
	msg_con_write,
	msg_con_open,
	msg_con_close,
	msg_con_read,
	msg_con_write,
	msg_tcgetattr,
	msg_tcsetattr,
	msg_tcgetsid,
	msg_tcflush,0
};

void con_panic(char *str, int error)
{
	scr_write("Console Manager: ");
	scr_write(str);
	scr_write("Exiting...\n");
	exit(error);
}

void *tty_worker(poolstruct_t pool)
{
	sigset_t set;
	struct sigaction action;
	int num, off;
	fptr_t ptr;

	sigfillset(&set);
	sigdelset(&set, TTY_REQUEST);
	pthread_sigmask(SIG_SETMASK, &set, NULL);

	action.sa_handler = SIG_IGN;
	sigaction(TTY_REQUEST, &action, NULL);
	pool->flag |= POOL_DONE;
	off = ((unsigned long)pool - (unsigned long)ttypool.pool)/sizeof(struct poolstruct);
 	syscall_change_iopl(3);

	while(1)
	{
		sigwait(&set, &num);

		ptr = con_func[pool->request];
		ptr(&pool->msg);
		clear_bit(&ttypool.pool_bitmap, off);
	}
}

/* Alternative worker */
void *worker(poolstruct_t pool)
{
	fptr_t ptr;

	ptr = con_func[pool->request];
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
	scr_write("OH serious error.");
	pthread_attr_init(&attr);
	parm.sched_priority = PRIO_SYS_DEF;
	pthread_attr_setschedparam(&attr, &parm);
	pthread_create(&thr->owner_id, &attr, (pthtype_t)worker, thr);
}

int main(int argc, char *argv[])
{
	int ret;

	termindex = 0;

	syscall_change_iopl(3);
	ttys = (struct terminal *) malloc(sizeof(struct terminal) * ntty);
	if(!ttys)
		con_panic("Not Enough Memory.\n", ENOMEM);

	ret = console_init();
	if(ret == -1)
		return 1;

	keyb_init();
	scr_write("CONMNGR: Initializing Console Manager: \n");
	ttypool.altworker = (pthtype_t)handle_full;
	ttypool.pool_bitmap = 0;
	ret = pool_init(&ttypool, MAX_TTY_THREADS, (pthtype_t)tty_worker);
	if(ret < 0)
		con_panic("Pool Failed", 1);

	msg_loop(&ttypool, TTY_REQUEST);

	return 0;
}

