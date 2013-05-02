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
 * It handles microkernel system calls
 */

#ifndef __ASM_SYSCALLDEFS_H
#define __ASM_SYSCALLDEFS_H

/* syscall numbers */
#define SYS_FORK		1
#define SYS_SPAWN		2
#define SYS_VFORK		3
#define SYS_EXEC		4
#define SYS_EXIT		5
#define SYS_GET_PID		6
#define SYS_GET_PPID		7
#define SYS_WAIT		8


#define SYS_GET_USER	9
#define SYS_SET_USER	10

#define SYS_CREATE_THREAD	11
#define SYS_DESTROY_THREAD	12
#define SYS_GET_TID			13
#define SYS_GET_USTACK		14
#define SYS_THREAD_JOIN		15

#define SYS_SCHED_GET		16
#define SYS_SCHED_SET		17
#define SYS_SCHED_YIELD		18

#define SYS_SIG_MASK		19
#define SYS_SIGNAL_ACTION	20
#define SYS_KILL			21
#define SYS_SIGNAL_SUSPEND	22
#define SYS_SIGNAL_STACK	23
#define SYS_SIGNAL_WAIT		24

#define SYS_VM_ALLOCATE		25
#define SYS_VM_DEALLOCATE	26
#define SYS_VM_MAP			27
#define SYS_VM_LOCK			28
#define SYS_VM_UNLOCK		29
#define SYS_VM_PROTECT		30

#define SYS_SEND_MESSAGE	31
#define SYS_RECEIVE_MESSAGE	32
#define SYS_REPLY_MESSAGE	33

#define SYS_IRQ_REQUEST		34
#define SYS_IRQ_FREE		35
#define SYS_IRQ_WAIT		36
#define SYS_LOG			37
#define SYS_CHANGE_IOPL		38

#define SYS_SYNC_CREATE		39
#define SYS_SYNC_DESTROY	40
#define SYS_SYNC_LOCK		41
#define SYS_SYNC_UNLOCK		42

#define SYS_FD_ALLOC		43
#define SYS_FD_FREE		44

#define SYS_SYSINFO		45
#define SYS_PIDLIST		46
#define SYS_PSTAT		47
#define SYS_TSTAT		48

#define SYS_SLEEP_UNINT		49
#define SYS_WAKE_UNINT		50


#define SYS_NSLEEP		51
#define SYS_GETTIME		52
#define SYS_SETTIME		53
#define SYS_CLOCK		54
#define SYS_GET_RESOLUTION	55
#define SYS_TIMER_CREATE	56
#define SYS_TIMER_DELETE	57
#define SYS_GET_TIMER		58
#define SYS_SET_TIMER		59
#define SYS_ALARM		60

#define SYS_DUP			61

#define SYS_GET_CWD		62
#define SYS_PUT_CWD		63

/* timer */
#define TM_TIMER			1



#endif /* __ASM_SYSCALL_H */
