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
 * File:- ipc.S
 */
 
#define __ASSEMBLY__
#include <asm/syscalldefs.h>

.global syscall_send_message
.global syscall_receive_message
.global syscall_reply_message

/*
	pushl %edx;		\
	popl %edx;		\
*/	

#define MSG_CONST		\
	pushl %ebp;		\
	pushl %esi;		\
	pushl %edi;		\
	pushl %ebx;		\
	pushl %eax;		\
	pushl %ecx;		\
	movl  28(%esp), %ebx;	\
	pushl  %ebx;		\
	movl 36(%esp), %ebx;	\
	movl 0(%ebx), %edi;	\
	movl 4(%ebx), %esi;	\
	movl 8(%ebx), %ebp;	\
	movl 20(%ebx), %edx;	\
	movl 24(%ebx), %ecx;	\
	xorl %eax, %eax;	\
	movw 30(%ebx), %ax;	\
	popl %ebx;		\
	shl $16, %eax; 


#define MSG_DEST		\
	int $0x30;		\
	pushl %eax;		\
	movl 36(%esp), %eax;	\
	movl %edi, 0(%eax);	\
	movl %esi, 4(%eax);	\
	movl %ebp, 8(%eax);	\
	movl %ebx, 16(%eax);	\
	movl %edx, 20(%eax);	\
	movl %ecx, 24(%eax);	\
	movl %eax, %ecx;	\
	popl %eax;		\
	movw %ax, 30(%ecx);	\
	popl %ecx;		\
	addl $4, %esp;		\
	popl %ebx;		\
	popl %edi;		\
	popl %esi;		\
	popl %ebp;		\
	ret

syscall_send_message:
	MSG_CONST
	
	orl $(SYS_SEND_MESSAGE), %eax
	
	MSG_DEST
	
syscall_receive_message:
	MSG_CONST
	
	orl $(SYS_RECEIVE_MESSAGE), %eax
	
	MSG_DEST

syscall_reply_message:
	MSG_CONST
	
	orl $(SYS_REPLY_MESSAGE), %eax

	MSG_DEST
	ret
