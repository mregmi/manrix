/**
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
 **/
/**
 * File:- message.c
 *		We are using Syncronous Message Passing. We have basically
 * three types of Messages. They are SHORT, LONG and MAP. The SHORT
 * messages are Passed Only in Registers. In LONG messages, the MEssages
 * are copied from One Adress Space to Another; This is not Possible
 * Directly so we are First copying to Kernel. Usually, The Messages
 * shorter than 256 Bytes are Long Transfered.
 * 	And the MAP messages are NOT actually Transferred but the Page tables are
 * Mapped.
 *
 **/

#include <kernel/message.h>
#include <kernel/thread.h>
#include <kernel/scheduling.h>
#include <kernel/kernel_stdio.h>

void ipc_init()
{

}

/* make the thread send blocked */
static inline void make_send_blocked(thread_t thr)
{
	thr->states = SEND_BLOCKED;
	remove_thread(thr);
	schedule();
}

/* make the thread reply blocked */
static inline void make_reply_blocked(thread_t thr)
{
	thr->states = REPLY_BLOCKED;
	remove_thread(thr);
	schedule();
}

/* make the thread receive blocked */
static inline void make_receive_blocked(thread_t thr)
{
	thr->states = RECEIVE_BLOCKED;
	remove_thread(thr);
	schedule();
}

/* Message passing */
static inline int transfer_msg(thread_t rcv, thread_t snd)
{
	u32 flags = snd->msg.flags;
	message_t msg = &snd->msg;
	vm_offset_t rcvptr = (vm_offset_t)msg->rcv_ptr, sndptr = (vm_offset_t)msg->snd_ptr;
	int ret = 0;

	if(flags & MSG_SHORT)
		return ret;

	else if(flags & MSG_MAP_RECEIVE)
	{
		ret = vm_share_addr(rcv->proc->map, snd->proc->map, &rcvptr, msg->rcv_bufsize, PAGE_RW|PAGE_USER);
		msg->rcv_ptr = (void *)rcvptr;
	}

	else if(flags & MSG_MAP_SEND)
	{
		ret = vm_share_addr(rcv->proc->map, snd->proc->map, &sndptr, msg->snd_bufsize, PAGE_USER);
		msg->snd_ptr = (void *)sndptr;
	}

	return ret;
}

int unmap_message(thread_t snd, thread_t rcv, message_t message)
{
	u32 flags = message->flags;
	message_t msg = &snd->msg;
	int ret = 0;

	if(flags & MSG_SHORT)
	{
		MSG_COPY_SHORT(&rcv->msg, message);
		return 0;
	}

/*	if(flags & MSG_MAP_RECEIVE)
	{
		ret = vm_unshare_addr(snd->proc->map, (vm_offset_t)msg->rcv_ptr, msg->rcv_bufsize);
	}

	/*
	 * FIXME: Removing the Mapping gives the fault.
	 */
/*	else if(flags & MSG_MAP_SEND)
	{
		ret = vm_unshare_addr(snd->proc->map, (vm_offset_t)msg->snd_ptr, msg->snd_bufsize);
		rcv->msg.w1 = message->w1;
		rcv->msg.w4 = message->w4;
		rcv->msg.w5 = message->w5;
	}
*/
	if(ret == -1)
		return -1;

	return 0;

}

error_t send_message(tid_t tid, message_t mesg)
{
	thread_t r_thr;
	thread_t sender = current;
	int ret;

	r_thr = find_by_tid(tid);
	if(!r_thr)
		return -ESRCH;
	sender->msg = *mesg;
	sender->msg.tid = sender->tid;

//	if(tid > 500)
//		kprintf("MEssage sent by %d %x\n", sender->tid, mesg->snd_bufsize);
	
	list_enqueue(&r_thr->rcvq, sender, sndq, thread_t);

	ret = transfer_msg(r_thr, sender);
	if(ret < 0)
	{
		kprintf("send_message: Transfer of message failed \n");
		return -ENOMEM;
	}

	if(r_thr->states & RECEIVE_BLOCKED)
		wakeup(r_thr);

	make_send_blocked(sender);

	*mesg = sender->msg;
	return sender->msg.w1;
}

error_t	receive_message(tid_t tid, message_t msg)
{
	thread_t rcv = current;
	thread_t snd;
	//kprintf("MEssage send by %d %x\n", sender->tid, mesg->snd_bufsize);
again:
	if(list_isempty(&rcv->rcvq))
	{
		make_receive_blocked(rcv);
		goto again;
	}
	else
	{

		list_dequeue(&rcv->rcvq, snd, sndq, thread_t);
		if(!snd)
		{
			kprintf("snder not found");
			return -1;
		}
/*		if((snd->msg.flags & MSG_LONG_SEND) && !(rcv->msg.flags & MSG_LONG_SEND))
		{
		kprintf(" oooops ");
			snd->msg.w1 = -1;
			wakeup(snd);
			return -1;
		}*/

		*msg = snd->msg;
//		kprintf("RCV MEssage send by %d %x\n", snd->tid, msg->snd_bufsize);
//		kprintf("MEssage received by  %d \n", msg->tid);
//		if(snd->msg.flags & MSG_MAP_SEND)
//			msg->rcv_ptr = snd->msg.snd_ptr;
//		else if(snd->msg.flags & MSG_MAP_RECEIVE)
//			msg->snd_ptr = snd->msg.rcv_ptr;

		if(snd->msg.flags & MSG_NOREPLY)
		{
			wakeup(snd);
			return 0;
		}

		snd->states = REPLY_BLOCKED;
	}
	return snd->tid;
}

int reply_message(tid_t tid, message_t msg)
{
	thread_t wait;
	thread_t snd = current;
	int ret;


	wait = find_by_tid(tid);
//	kprintf("MEssage replyed to %d", wait->tid);
	if((wait == NULL) || !(wait->states & REPLY_BLOCKED))
		return -ESRCH;

	wait->msg = *msg;
/*	if(msg->flags & MSG_LONG_RECEIVE)
	{
		wait->msg.rcv_ptr = msg->snd_ptr;
		wait->msg.rcv_bufsize = msg->snd_bufsize;
	}
*/

	ret = unmap_message(snd, wait, msg);
	if(ret < 0)
		kprintf("Reply_message: Reply failed\n");

	wakeup(wait);

	return ret;
}

