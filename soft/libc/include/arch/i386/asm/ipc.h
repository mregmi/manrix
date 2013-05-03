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
 * File: ipc.h
 * 	ManRiX IPC mechanism declarations.
 */

#ifndef __ASM_IPC_H
#define __ASM_IPC_H

#include <mrx/fscases.h>

#define MSG_SHORT		1
#define MSG_LONG_SEND	2
#define MSG_MAP_SEND	4
#define MSG_MAP_RECEIVE 8
#define MSG_BOTHBUF		0xC
#define MSG_NOREPLY		0x20
#define MSG_LONG_RECEIVE	0x40

#define MAX_MSG_LONG	256

#define MSG_COPY_SHORT(to, from)	(*(to) = *(from))


/* DO not change the order of the elements. The elements are laid according
 * to the layout of regs in stack.
 * eg on x86 see interrupt.S. The messages are in regs as
 * EDI:- sender's buffer size or word3
 * ESI:- receiver's pointer or word4
 * EBP:- receiver's size or word5
 * ESP:- Not used.
 * EBX:- tid
 * EDX:- sender's pointer or word2
 * ECX:- word1
 * EAX:- flag & syscall Number
 * Pusha pushes the regs in this order (reversed).
 */
struct message{
	union {
 		unsigned long w3;
		unsigned long snd_bufsize;
	}m1;

	union {
		unsigned long w4;
		void *rcv_ptr;
	}m2;

	union {
		unsigned long w5;
		unsigned long rcv_bufsize;
	}m3;

	unsigned long dummy1;	/* reserved. */
	int tid;

	union {
		unsigned long w2;
		void *snd_ptr;
	}m4;

	unsigned long w1;
	unsigned short dummy, flags;

#define w2			m4.w2
#define snd_ptr		m4.snd_ptr

#define w3			m1.w3
#define snd_bufsize	m1.snd_bufsize

#define w4			m2.w4
#define rcv_ptr		m2.rcv_ptr

#define w5			m3.w5
#define rcv_bufsize	m3.rcv_bufsize

};

typedef struct message *message_t;

extern int send_message(int , message_t );
extern int receive_message(int , message_t );
extern int reply_message(int , message_t );

extern int syscall_receive_message(int , message_t );
extern int syscall_send_message(int , message_t );
extern int syscall_reply_message(int , message_t );

#endif /* __IPC_H */
