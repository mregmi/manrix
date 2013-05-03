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
/*	file: unistd.h
 * 		standard symbolic constants and types as defind by POSIX.
 */


#ifndef __UNISTD_H
#define __UNISTD_H

#ifndef __SYS_TYPES_H
 #include <sys/types.h>
#endif

/*
 * Integer value indicating version of IEEE Std 1003.1 (C-language binding) to which
 * the implementation conforms. For implementations conforming to IEEE Std 1003.1-2001,
 * the value shall be 200112L.
 */
#define _POSIX_VERSION

/*
 * Integer value indicating version of the Shell and Utilities volume of IEEE Std 1003.1
 * to which the implementation conforms. For implementations conforming to
 * IEEE Std 1003.1-2001, the value shall be 200112L.
 * The following symbolic constant shall be defined only if the implementation supports
 * the XSI option; see XSI Conformance .
 */
#define _POSIX2_VERSION

/*
 * Integer value indicating version of the X/Open Portability Guide to which the
 * implementation conforms. The value shall be 600.
 */
#define _XOPEN_VERSION



/*
 * The implementation supports the Advisory Information option. If this symbol is
 * defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L. The value of
 * this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_ASYNCHRONOUS_IO				0

/*
 * The implementation supports the Asynchronous Input and Output option. If this
 * symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_ADVISORY_INFO				0

/*
 * The implementation supports the Barriers option. If this symbol is defined in
 * <unistd.h>, it shall be defined to be -1, 0, or 200112L. The value of this
 * symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_BARRIERS					0

/*
 * The use of chown() and fchown() is restricted to a process with appropriate
 * privileges, and to changing the group ID of a file only to the effective group
 * ID of the process or to one of its supplementary group IDs. This symbol shall
 * either be undefined or defined with a value other than -1.
 */
#define _POSIX_CHOWN_RESTRICTED

/*
 * The implementation supports the Clock Selection option. If this symbol is
 * defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_CLOCK_SELECTION

/*
 * The implementation supports the Process CPU-Time Clocks option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_CPUTIME

/*
 * The implementation supports the File Synchronization option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_FSYNC

/*
 * The implementation supports the IPv6 option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_IPV6

/*
 * The implementation supports job control. This symbol shall always be set
 * to a value greater than zero.
 */
#define _POSIX_JOB_CONTROL

/*
 * The implementation supports the Memory Mapped Files option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_MAPPED_FILES

/*
 * The implementation supports the Process Memory Locking option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_MEMLOCK

/*
 * The implementation supports the Range Memory Locking option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_MEMLOCK_RANGE

/*
 * The implementation supports the Memory Protection option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_MEMORY_PROTECTION

/*
 * The implementation supports the Message Passing option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_MESSAGE_PASSING

/*
 * The implementation supports the Monotonic Clock option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_MONOTONIC_CLOCK

/*
 * Pathname components longer than {NAME_MAX} generate an error.
 * This symbol shall either be undefined or defined with a value other than -1.
 */
#define _POSIX_NO_TRUNC

/*
 * The implementation supports the Prioritized Input and Output option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_PRIORITIZED_IO

/*
 * The implementation supports the Process Scheduling option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_PRIORITY_SCHEDULING

/* The implementation supports the Raw Sockets option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_RAW_SOCKETS

/*
 * The implementation supports the Read-Write Locks option.
 * This is always set to a value greater than zero if the Threads option is supported.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_READER_WRITER_LOCKS

/*
 * The implementation supports the Realtime Signals Extension option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_REALTIME_SIGNALS					200112L

/*
 * The implementation supports the Regular Expression Handling option.
 * This symbol shall always be set to a value greater than zero.
 */
#define _POSIX_REGEXP

/*
 * Each process has a saved set-user-ID and a saved set-group-ID.
 * This symbol shall always be set to a value greater than zero.
 */
#define _POSIX_SAVED_IDS

/*
 * The implementation supports the Semaphores option. If this symbol is
 * defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_SEMAPHORES						200112L

/*
 * The implementation supports the Shared Memory Objects option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_SHARED_MEMORY_OBJECTS

/*
 * The implementation supports the POSIX shell. This symbol shall always be set to a
 * value greater than zero.
 */
#define _POSIX_SHELL

/*
 * The implementation supports the Spawn option. If this symbol is defined in <unistd.h>,
 * it shall be defined to be -1, 0, or 200112L. The value of this symbol reported
 * by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_SPAWN

/*
 * The implementation supports the Spin Locks option. If this symbol is
 * defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_SPIN_LOCKS

/*
 * The implementation supports the Process Sporadic Server option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_SPORADIC_SERVER

/*
 * The implementation supports the Synchronized Input and Output option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_SYNCHRONIZED_IO

/*
 * The implementation supports the Thread Stack Address Attribute option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_THREAD_ATTR_STACKADDR			200112L

/*
 * The implementation supports the Thread Stack Size Attribute option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_THREAD_ATTR_STACKSIZE			200112L

/*
 * The implementation supports the Thread CPU-Time Clocks option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_THREAD_CPUTIME

/*
 * The implementation supports the Thread Priority Inheritance option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_THREAD_PRIO_INHERIT

/*
 * The implementation supports the Thread Priority Protection option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_THREAD_PRIO_PROTECT

/*
 * The implementation supports the Thread Execution Scheduling option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_THREAD_PRIORITY_SCHEDULING

/*
 * The implementation supports the Thread Process-Shared Synchronization option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_THREAD_PROCESS_SHARED

/*
 * The implementation supports the Thread-Safe Functions option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_THREAD_SAFE_FUNCTIONS

/*
 * The implementation supports the Thread Sporadic Server option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_THREAD_SPORADIC_SERVER

/*
 * The implementation supports the Threads option. If this symbol is
 * defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_THREADS							200112L

/*
 * The implementation supports the Timeouts option. If this symbol is
 * defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_TIMEOUTS

/*
 * The implementation supports the Timers option. If this symbol is
 * defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_TIMERS							200112L

/*
 * The implementation supports the Trace option. If this symbol is
 * defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_TRACE

/*
 * The implementation supports the Trace Event Filter option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_TRACE_EVENT_FILTER

/*
 * The implementation supports the Trace Inherit option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_TRACE_INHERIT

/*
 * The implementation supports the Trace Log option. If this symbol is
 * defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_TRACE_LOG

/*
 * The implementation supports the Typed Memory Objects option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX_TYPED_MEMORY_OBJECTS

/*
 * This symbol shall be defined to be the value of a character that shall
 * disable terminal special character handling as described in <termios.h> .
 * This symbol shall always be set to a value other than -1.
 */
#define _POSIX_VDISABLE

/*
 * The implementation supports the C-Language Binding option.
 * This symbol shall always have the value 200112L.
 */
#define _POSIX2_C_BIND						200112L

/*
 * The implementation supports the C-Language Development Utilities option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX2_C_DEV

/*
 * The implementation supports at least one terminal type.
 */
#define _POSIX2_CHAR_TERM

/*
 * The implementation supports the FORTRAN Development Utilities option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX2_FORT_DEV

/*
 * The implementation supports the FORTRAN Runtime Utilities option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX2_FORT_RUN

/*
 * The implementation supports the creation of locales by the localedef utility.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX2_LOCALEDEF

/*
 * The implementation supports the Batch Environment Services and Utilities option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX2_PBS

/*
 * The implementation supports the Batch Accounting option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX2_PBS_ACCOUNTING

/*
 * The implementation supports the Batch Checkpoint/Restart option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX2_PBS_CHECKPOINT

/*
 * The implementation supports the Locate Batch Job Request option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX2_PBS_LOCATE

/*
 * The implementation supports the Batch Job Message Request option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX2_PBS_MESSAGE

/*
 * The implementation supports the Track Batch Job Request option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX2_PBS_TRACK

/*
 * The implementation supports the Software Development Utilities option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX2_SW_DEV

/*
 * The implementation supports the User Portability Utilities option.
 * If this symbol is defined in <unistd.h>, it shall be defined to be -1, 0, or 200112L.
 * The value of this symbol reported by sysconf() shall either be -1 or 200112L.
 */
#define _POSIX2_UPE

/*
 * The implementation provides a C-language compilation environment with
 * 32-bit int, long, pointer, and off_t types.
 */
#define _POSIX_V6_ILP32_OFF32		1

/*
 * The implementation provides a C-language compilation environment with
 * 32-bit int, long, and pointer types and an off_t type using at least 64 bits.
 */
#define _POSIX_V6_ILP32_OFFBIG		0

/*
 * The implementation provides a C-language compilation environment with
 * 32-bit int and 64-bit long, pointer, and off_t types.
 */
#define _POSIX_V6_LP64_OFF64		0

/*
 * The implementation provides a C-language compilation environment with
 * an int type using at least 32 bits and long, pointer, and off_t types using at least 64 bits.
 */
#define _POSIX_V6_LPBIG_OFFBIG		0

/*
 * The implementation provides a C-language compilation environment with
 * 32-bit int, long, pointer, and off_t types.
 */
#define _XBS5_ILP32_OFF32		1

/*
 * The implementation provides a C-language compilation environment with
 * 32-bit int, long, and pointer types and an off_t type using at least 64 bits.
 */
#define _XBS5_ILP32_OFFBIG		0


/*
 * The implementation provides a C-language compilation environment with
 * 32-bit int and 64-bit long, pointer, and off_t types.
 */
#define _XBS5_LP64_OFF64		0

/*
 * The implementation provides a C-language compilation environment with
 * an int type using at least 32 bits and long, pointer, and off_t types using at least 64 bits.
 */
#define _XBS5_LPBIG_OFFBIG		0

/*
 * The implementation supports the X/Open Encryption Option Group.
 */
#define _XOPEN_CRYPT			0

/*
 * The implementation supports the Issue 4, Version 2 Enhanced
 * Internationalization Option Group. This symbol shall always be set to a value other than -1.
 */
#define _XOPEN_ENH_I18N			0

/*
 * The implementation supports the Legacy Option Group.
 */
#define _XOPEN_LEGACY			1

/*
 * The implementation supports the X/Open Realtime Option Group.
 */
#define _XOPEN_REALTIME			0

/*
 * The implementation supports the X/Open Realtime Threads Option Group.
 */
#define _XOPEN_REALTIME_THREADS		1

/*
 * The implementation supports the Issue 4, Version 2 Shared Memory Option Group.
 * This symbol shall always be set to a value other than -1.
 */
#define _XOPEN_SHM			0

/*
 * The implementation supports the XSI STREAMS Option Group.
 */
#define _XOPEN_STREAMS			0

/*
 * The implementation supports the XSI extension.
 */
#define _XOPEN_UNIX			1

/*
 * Asynchronous input or output operations may be performed for the associated file.
 */
#define _POSIX_ASYNC_IO			0

/*
 * Prioritized input or output operations may be performed for the associated file.
 */
#define _POSIX_PRIO_IO

/*
 * Synchronized input or output operations may be performed for the associated file.
 */
#define _POSIX_SYNC_IO

/*
 * Null Pointer
 */
#ifndef NULL
#define NULL		((void *)0)
#endif	/* NULL */

#define F_OK		1	/* Test for existence of file. */
#define R_OK		2	/* Test for read permission. */
#define W_OK		4	/* Test for write permission. */
#define X_OK		8	/* Test for execute (search) permission */


/*
 * This is the value for the PATH environment variable that finds all standard utilities.
 */
#define _CS_PATH		1

/*
 * If sysconf(_SC_V6_ILP32_OFF32) returns -1, the meaning of this value is unspecified.
 * Otherwise, this value is the set of initial options to be given to the c99 utility
 * to build an application using a programming model with 32-bit int, long, pointer, and off_t types.
 */
#define _CS_POSIX_V6_ILP32_OFF32_CFLAGS		2

/*
 * If sysconf(_SC_V6_ILP32_OFF32) returns -1, the meaning of this value is unspecified.
 * Otherwise, this value is the set of final options to be given to the c99 utility to
 * build an application using a programming model with 32-bit int, long, pointer, and off_t types.
 */
#define _CS_POSIX_V6_ILP32_OFF32_LDFLAGS	3

/*
 * If sysconf(_SC_V6_ILP32_OFF32) returns -1, the meaning of this value is unspecified.
 * Otherwise, this value is the set of libraries to be given to the c99 utility to build
 * an application using a programming model with 32-bit int, long, pointer, and off_t types.
 */
#define _CS_POSIX_V6_ILP32_OFF32_LIBS		4

/*
 * If sysconf(_SC_V6_ILP32_OFFBIG) returns -1, the meaning of this value is unspecified.
 * Otherwise, this value is the set of initial options to be given to the c99 utility to
 * build an application using a programming model with 32-bit int, long, and pointer types,
 * and an off_t type using at least 64 bits.
 */
#define _CS_POSIX_V6_ILP32_OFFBIG_CFLAGS		5

/*
 * If sysconf(_SC_V6_ILP32_OFFBIG) returns -1, the meaning of this value is unspecified.
 * Otherwise, this value is the set of final options to be given to the c99 utility to
 * build an application using a programming model with 32-bit int, long, and pointer types,
 * and an off_t type using at least 64 bits.
 */
#define _CS_POSIX_V6_ILP32_OFFBIG_LDFLAGS		6

/*
 * If sysconf(_SC_V6_ILP32_OFFBIG) returns -1, the meaning of this value is unspecified.
 * Otherwise, this value is the set of libraries to be given to the c99 utility to build
 * an application using a programming model with 32-bit int, long, and pointer types,
 * and an off_t type using at least 64 bits.
 */
#define _CS_POSIX_V6_ILP32_OFFBIG_LIBS		7

/*
 * If sysconf(_SC_V6_LP64_OFF64) returns -1, the meaning of this value is unspecified.
 * Otherwise, this value is the set of initial options to be given to
 * the c99 utility to build an application using a programming model with 32-bit int
 * and 64-bit long, pointer, and off_t types.
 */
#define _CS_POSIX_V6_LP64_OFF64_CFLAGS		8

/*
 * If sysconf(_SC_V6_LP64_OFF64) returns -1, the meaning of this value is unspecified.
 * Otherwise, this value is the set of final options to be given to the c99 utility
 * to build an application using a programming model with 32-bit int and 64-bit long,
 * pointer, and off_t types.
 */
#define _CS_POSIX_V6_LP64_OFF64_LDFLAGS		9

/*
 * If sysconf(_SC_V6_LP64_OFF64) returns -1, the meaning of this value is unspecified.
 * Otherwise, this value is the set of libraries to be given to the c99 utility to build
 * an application using a programming model with 32-bit int and 64-bit long, pointer,
 * and off_t types.
 */
#define _CS_POSIX_V6_LP64_OFF64_LIBS		10

/*
 * If sysconf(_SC_V6_LPBIG_OFFBIG) returns -1, the meaning of this value is unspecified.
 * Otherwise, this value is the set of initial options to be given to the c99 utility
 * to build an application using a programming model with an int type using at least 32 bits
 * and long, pointer, and off_t types using at least 64 bits.
 */
#define _CS_POSIX_V6_LPBIG_OFFBIG_CFLAGS		11

/*
 * If sysconf(_SC_V6_LPBIG_OFFBIG) returns -1, the meaning of this value is unspecified.
 * Otherwise, this value is the set of final options to be given to the c99 utility to
 * build an application using a programming model with an int type using at least 32 bits
 * and long, pointer, and off_t types using at least 64 bits.
 */
#define _CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGS		12

/*
 * If sysconf(_SC_V6_LPBIG_OFFBIG) returns -1, the meaning of this value is unspecified.
 * Otherwise, this value is the set of libraries to be given to the c99 utility to build
 * an application using a programming model with an int type using at least 32 bits and
 * long, pointer, and off_t types using at least 64 bits.
 */
#define _CS_POSIX_V6_LPBIG_OFFBIG_LIBS		13

/*
 * This value is a <newline>-separated list of names of programming environments
 * supported by the implementation in which the widths of the blksize_t, cc_t, mode_t,
 * nfds_t, pid_t, ptrdiff_t, size_t, speed_t, ssize_t, suseconds_t, tcflag_t, useconds_t,
 * wchar_t, and wint_t types are no greater than the width of type long.
 * The format of each name shall be suitable for use with the getconf -v option.
 */
#define _CS_POSIX_V6_WIDTH_RESTRICTED_ENVS		14
#define _CS_V6_WIDTH_RESTRICTED_ENVS	_CS_POSIX_V6_WIDTH_RESTRICTED_ENVS

#define _CS_XBS5_ILP32_OFF32_CFLAGS 			15/* (LEGACY) */
#define _CS_XBS5_ILP32_OFF32_LDFLAGS 			16/* (LEGACY) */
#define _CS_XBS5_ILP32_OFF32_LIBS				17/* (LEGACY) */
#define _CS_XBS5_ILP32_OFF32_LINTFLAGS 			18/* (LEGACY) */
#define _CS_XBS5_ILP32_OFFBIG_CFLAGS 			19/* (LEGACY) */
#define _CS_XBS5_ILP32_OFFBIG_LDFLAGS 			20/* (LEGACY) */
#define _CS_XBS5_ILP32_OFFBIG_LIBS 				21/* (LEGACY) */
#define _CS_XBS5_ILP32_OFFBIG_LINTFLAGS 		22/* (LEGACY) */
#define _CS_XBS5_LP64_OFF64_CFLAGS 				23/* (LEGACY) */
#define _CS_XBS5_LP64_OFF64_LDFLAGS 			24/* (LEGACY) */
#define _CS_XBS5_LP64_OFF64_LIBS 				25/* (LEGACY) */
#define _CS_XBS5_LP64_OFF64_LINTFLAGS 			26/* (LEGACY) */
#define _CS_XBS5_LPBIG_OFFBIG_CFLAGS 			27/* (LEGACY) */
#define _CS_XBS5_LPBIG_OFFBIG_LDFLAGS 			28/* (LEGACY) */
#define _CS_XBS5_LPBIG_OFFBIG_LIBS 				29/* (LEGACY) */
#define _CS_XBS5_LPBIG_OFFBIG_LINTFLAGS 		30/* (LEGACY) */

/* defined for the lseek() and fcntl() functions  */
#ifndef SEEK_CUR
#define SEEK_CUR 	1	/* Set file offset to current plus offset. */
#endif

#ifndef SEEK_END
#define SEEK_END 	2	/* Set file offset to EOF plus offset.  */
#endif

#ifndef SEEK_SET
#define SEEK_SET 	3	/* Set file offset to offset. */
#endif

/* defined as possible values for the function argument to the lockf() function. */
#define F_LOCK 			/* Lock a section for exclusive use. */
#define F_TEST 			/* Test section for locks by other processes. */
#define F_TLOCK 		/* Test and lock a section for exclusive use. */
#define F_ULOCK 		/* Unlock locked sections.  */


/* defined for pathconf(). */
#define _PC_2_SYMLINKS
#define _PC_ALLOC_SIZE_MIN
#define _PC_ASYNC_IO
#define _PC_CHOWN_RESTRICTED
#define _PC_FILESIZEBITS
#define _PC_LINK_MAX
#define _PC_MAX_CANON
#define _PC_MAX_INPUT
#define _PC_NAME_MAX
#define _PC_NO_TRUNC
#define _PC_PATH_MAX
#define _PC_PIPE_BUF
#define _PC_PRIO_IO
#define _PC_REC_INCR_XFER_SIZE
#define _PC_REC_MIN_XFER_SIZE
#define _PC_REC_XFER_ALIGN
#define _PC_SYMLINK_MAX
#define _PC_SYNC_IO
#define _PC_VDISABLE

/* defined for sysconf(). */
/*
#define _SC_2_C_BIND			1
#define _SC_2_C_DEV			2
#define _SC_2_CHAR_TERM			3
#define _SC_2_FORT_DEV			4
#define _SC_2_FORT_RUN			5
#define _SC_2_LOCALEDEF			6
#define _SC_2_PBS			7
#define _SC_2_PBS_ACCOUNTING		8
#define _SC_2_PBS_CHECKPOINT		9
#define _SC_2_PBS_LOCATE		10
#define _SC_2_PBS_MESSAGE		11
#define _SC_2_PBS_TRACK			12
#define _SC_2_SW_DEV			13
#define _SC_2_UPE			14
#define _SC_2_VERSION			15
#define _SC_ADVISORY_INFO		16
#define _SC_AIO_LISTIO_MAX		17
#define _SC_AIO_MAX			18
#define _SC_AIO_PRIO_DELTA_MAX		19
#define _SC_ARG_MAX			20
#define _SC_ASYNCHRONOUS_IO		21
#define _SC_ATEXIT_MAX			22
#define _SC_BARRIERS			23
#define _SC_BC_BASE_MAX			24
#define _SC_BC_DIM_MAX			25
#define _SC_BC_SCALE_MAX		26
#define _SC_BC_STRING_MAX		27
#define _SC_CHILD_MAX			28
#define _SC_CLK_TCK			29
#define _SC_CLOCK_SELECTION		30
#define _SC_COLL_WEIGHTS_MAX		31
#define _SC_CPUTIME			32
#define _SC_DELAYTIMER_MAX		33
#define _SC_EXPR_NEST_MAX		34
#define _SC_FSYNC			35
#define _SC_GETGR_R_SIZE_MAX		36
#define _SC_GETPW_R_SIZE_MAX		37
#define _SC_HOST_NAME_MAX		38
#define _SC_IOV_MAX			39
#define _SC_IPV6			40
#define _SC_JOB_CONTROL			41
#define _SC_LINE_MAX			42
#define _SC_LOGIN_NAME_MAX		43
#define _SC_MAPPED_FILES		44
#define _SC_MEMLOCK			45
#define _SC_MEMLOCK_RANGE		46
#define _SC_MEMORY_PROTECTION		47
#define _SC_MESSAGE_PASSING		48
#define _SC_MONOTONIC_CLOCK		49
#define _SC_MQ_OPEN_MAX			50
#define _SC_MQ_PRIO_MAX			51
#define _SC_NGROUPS_MAX			52
#define _SC_OPEN_MAX			53
#define _SC_PAGE_SIZE			54
#define _SC_PAGESIZE			55
#define _SC_PRIORITIZED_IO
#define _SC_PRIORITY_SCHEDULING
#define _SC_RAW_SOCKETS
#define _SC_RE_DUP_MAX
#define _SC_READER_WRITER_LOCKS
#define _SC_REALTIME_SIGNALS
#define _SC_REGEXP
#define _SC_RTSIG_MAX
#define _SC_SAVED_IDS
#define _SC_SEM_NSEMS_MAX
#define _SC_SEM_VALUE_MAX
#define _SC_SEMAPHORES
#define _SC_SHARED_MEMORY_OBJECTS
#define _SC_SHELL
#define _SC_SIGQUEUE_MAX
#define _SC_SPAWN
#define _SC_SPIN_LOCKS
#define _SC_SPORADIC_SERVER
#define _SC_SS_REPL_MAX
#define _SC_STREAM_MAX
#define _SC_SYMLOOP_MAX
#define _SC_SYNCHRONIZED_IO
#define _SC_THREAD_ATTR_STACKADDR
#define _SC_THREAD_ATTR_STACKSIZE
#define _SC_THREAD_CPUTIME
#define _SC_THREAD_DESTRUCTOR_ITERATIONS
#define _SC_THREAD_KEYS_MAX
#define _SC_THREAD_PRIO_INHERIT
#define _SC_THREAD_PRIO_PROTECT
#define _SC_THREAD_PRIORITY_SCHEDULING
#define _SC_THREAD_PROCESS_SHARED
#define _SC_THREAD_SAFE_FUNCTIONS
#define _SC_THREAD_SPORADIC_SERVER
#define _SC_THREAD_STACK_MIN
#define _SC_THREAD_THREADS_MAX
#define _SC_THREADS
#define _SC_TIMEOUTS
#define _SC_TIMER_MAX
#define _SC_TIMERS
#define _SC_TRACE
#define _SC_TRACE_EVENT_FILTER
#define _SC_TRACE_EVENT_NAME_MAX
#define _SC_TRACE_INHERIT
#define _SC_TRACE_LOG
#define _SC_TRACE_NAME_MAX
#define _SC_TRACE_SYS_MAX
#define _SC_TRACE_USER_EVENT_MAX
#define _SC_TTY_NAME_MAX
#define _SC_TYPED_MEMORY_OBJECTS
#define _SC_TZNAME_MAX
#define _SC_V6_ILP32_OFF32
#define _SC_V6_ILP32_OFFBIG
#define _SC_V6_LP64_OFF64
#define _SC_V6_LPBIG_OFFBIG
#define _SC_VERSION
#define _SC_XBS5_ILP32_OFF32
#define _SC_XBS5_ILP32_OFFBIG
#define _SC_XBS5_LP64_OFF64
#define _SC_XBS5_LPBIG_OFFBIG
#define _SC_XOPEN_CRYPT
#define _SC_XOPEN_ENH_I18N
#define _SC_XOPEN_LEGACY
#define _SC_XOPEN_REALTIME
#define _SC_XOPEN_REALTIME_THREADS
#define _SC_XOPEN_SHM
#define _SC_XOPEN_STREAMS
#define _SC_XOPEN_UNIX
#define _SC_XOPEN_VERSION
*/

#define STDERR_FILENO 		2	/* File descriptor number of stderr; 2. */
#define STDIN_FILENO 		0	/* File descriptor number of stdin; 0.  */
#define STDOUT_FILENO 		1	/* File descriptor number of stdout; 1. */

extern char  *optarg;
extern int    optind, opterr, optopt;
extern char **environ;

#ifndef __environ
 #define __environ environ
#endif

/*
 * access - determine accessibility of a file
 */
int          access(const char *, int);

/*
 * alarm - schedule an alarm signal
 */
unsigned     alarm(unsigned);

/*
 * chdir - change working directory
 */
int          chdir(const char *);

/*
 * chown - change owner and group of a file
 */
int          chown(const char *, uid_t, gid_t);


int          close(int);
size_t       confstr(int, char *, size_t);


char        *crypt(const char *, const char *);
char        *ctermid(char *);

/*
 * dup - duplicate an open file descriptor
 */
int          dup(int);

/*
 * dup2 - duplicate an open file descriptor
 */
int          dup2(int, int);

void         encrypt(char[64], int);

int          execl(const char *, const char *, ...);
int          execle(const char *, const char *, ...);
int          execlp(const char *, const char *, ...);
int          execv(const char *, char *const []);
int          execve(const char *, char *const [], char *const []);
int          execvp(const char *, char *const []);
void        _exit(int);
int          fchown(int, uid_t, gid_t);

int          fchdir(int);


int          fdatasync(int);

/*
 * fork - create a new process
 */
pid_t        fork(void);
long         fpathconf(int, int);

int          fsync(int);

int          ftruncate(int, off_t);

/*
 * getcwd - get the pathname of the current working directory
 */
char        *getcwd(char *, size_t);

/*
 * getegid - get the effective group ID
 */
gid_t        getegid(void);

/*
 * geteuid - get the effective user ID
 */
uid_t        geteuid(void);

/*
 * getgid - get real-group-ID
 */
gid_t        getgid(void);
int          getgroups(int, gid_t []);

long         gethostid(void);

int          gethostname(char *, size_t);
char        *getlogin(void);
int          getlogin_r(char *, size_t);
int          getopt(int, char * const [], const char *);

/*
 * getpgid - get the process group ID for a process
 */
pid_t        getpgid(pid_t);

/*
 * getpgrp - get the process group ID of the calling process
 */
pid_t        getpgrp(void);
pid_t        getpid(void);
pid_t        getppid(void);

pid_t        getsid(pid_t);

/*
 * getuid - get a real user ID
 */
uid_t        getuid(void);

char        *getwd(char *);

int          isatty(int);

int          lchown(const char *, uid_t, gid_t);

int          link(const char *, const char *);

int          lockf(int, int, off_t);

off_t        lseek(int, off_t, int);

int          nice(int);

long         pathconf(const char *, int);
int          pause(void);
int          pipe(int [2]);

ssize_t      pread(int, void *, size_t, off_t);
ssize_t      pwrite(int, const void *, size_t, off_t);

ssize_t      read(int, void *, size_t);
ssize_t      readlink(const char *__restrict, char *__restrict, size_t);
int          rmdir(const char *);

/*
 * setegid - set effective-group-ID
 */
int          setegid(gid_t);

/*
 * seteuid - set effective-user-ID
 */
int          seteuid(uid_t);

/*
 * setgid - set-group-ID
 */
int          setgid(gid_t);

/*
 * setpgid - set process group ID for job control
 */
int          setpgid(pid_t, pid_t);

pid_t        setpgrp(void);
int          setregid(gid_t, gid_t);
int          setreuid(uid_t, uid_t);

pid_t        setsid(void);

/*
 * setuid - set real-user-ID
 */
int          setuid(uid_t);
unsigned     sleep(unsigned);

void         swab(const void *__restrict, void *__restrict, ssize_t);

int          symlink(const char *, const char *);

void         sync(void);

long         sysconf(int);
pid_t        tcgetpgrp(int);
int          tcsetpgrp(int, pid_t);

int          truncate(const char *, off_t);

char        *ttyname(int);
int          ttyname_r(int, char *, size_t);

useconds_t   ualarm(useconds_t, useconds_t);

int          unlink(const char *);
int          usleep(useconds_t);

/*
 * vfork - create a new process; share virtual memory.
 * In manrix it is just an inlined call to syscall.
 */
#include <asm/syscall.h> 
//pid_t        vfork(void);

ssize_t      write(int, const void *, size_t);

#endif  /* __UNISTD_H  */

