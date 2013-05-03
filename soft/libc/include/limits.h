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
 * File: limits.h
 * 		- implementation-defined constants
 */

#ifndef __LIMITS_H
#define __LIMITS_H

/* Some are Really Architecture Dependent */
#include <asm/limits.h>

/* Runtime Invariant Values (Possibly Indeterminate) */

/*
 * Maximum number of I/O operations in a single list I/O call
 * supported by the implementation.
 */
#define AIO_LISTIO_MAX

/* Maximum number of outstanding asynchronous I/O operations
 * supported by the implementation.
 */
#define AIO_MAX

/*
 * The maximum amount by which a process can decrease its
 * asynchronous I/O priority level from its own scheduling priority.
 */
#define AIO_PRIO_DELTA_MAX

/*
 * Maximum length of argument to the exec functions
 * including environment data.
 */
#define ARG_MAX

/*
 * Maximum number of functions that may be registered with atexit().
 */
#define ATEXIT_MAX

/*
 * Maximum number of simultaneous processes per real user ID.
 */
#define CHILD_MAX

/*
 * Maximum number of timer expiration overruns.
 */
#define DELAYTIMER_MAX

/*
 * Maximum length of a host name (not including the terminating null)
 * as returned from the gethostname() function.
 */
#define HOST_NAME_MAX

/*
 * Maximum number of iovec structures that one process has
 * available for use with readv() or writev().
 */
#define IOV_MAX

/*
 * Maximum length of a login name.
 */
#define LOGIN_NAME_MAX

/*
 * The maximum number of open message queue descriptors
 * a process may hold.
 */
#define MQ_OPEN_MAX

/*
 * The maximum number of message priorities supported by the implementation.
 */
#define MQ_PRIO_MAX

/*
 * Maximum number of files that one process can have open at any one time.
 */
#define OPEN_MAX

/*
 * Size in bytes of a page.
 */
#define PAGESIZE
#define PAGE_SIZE	PAGESIZE

/*
 * Maximum number of attempts made to destroy a thread's
 * thread-specific data values on thread exit.
 */
#define PTHREAD_DESTRUCTOR_ITERATIONS

/*
 * Maximum number of data keys that can be created by a process.
 */
#define PTHREAD_KEYS_MAX

/*
 * Minimum size in bytes of thread stack storage.
 */
#ifndef PTHREAD_STACK_MIN 
#define PTHREAD_STACK_MIN	PAGE_SIZE
#endif

/*
 * Maximum number of threads that can be created per process.
 */
#define PTHREAD_THREADS_MAX

/*
 * The number of repeated occurrences of a BRE permitted
 * by the regexec() and regcomp() functions when using the interval
 * notation {\(m,n\}; see BREs Matching Multiple Characters .
 */
#define RE_DUP_MAX

/*
 * Maximum number of realtime signals reserved for application use
 * in this implementation.
 */
#define RTSIG_MAX

/*
 * Maximum number of semaphores that a process may have.
 */
#define SEM_NSEMS_MAX

/*
 * The maximum value a semaphore may have.
 */
#define SEM_VALUE_MAX

#define SIGQUEUE_MAX
/*[RTS]
Maximum number of queued signals that a process may send and have pending at the receiver(s) at any time.
Minimum Acceptable Value: #define _POSIX_SIGQUEUE_MAX}
#define SS_REPL_MAX}
[SS|TSP]
The maximum number of replenishment operations that may be simultaneously pending for a particular sporadic server scheduler.
Minimum Acceptable Value: #define _POSIX_SS_REPL_MAX}
#define STREAM_MAX}
The number of streams that one process can have open at one time. If defined, it has the same value as {FOPEN_MAX} (see <stdio.h> ).
Minimum Acceptable Value: #define _POSIX_STREAM_MAX}
#define SYMLOOP_MAX}
Maximum number of symbolic links that can be reliably traversed in the resolution of a pathname in the absence of a loop.
Minimum Acceptable Value: #define _POSIX_SYMLOOP_MAX}
#define TIMER_MAX}
[TMR]
Maximum number of timers per process supported by the implementation.
Minimum Acceptable Value: #define _POSIX_TIMER_MAX}
#define TRACE_EVENT_NAME_MAX}
[TRC]
Maximum length of the trace event name.
Minimum Acceptable Value: #define _POSIX_TRACE_EVENT_NAME_MAX}
#define TRACE_NAME_MAX}
[TRC]
Maximum length of the trace generation version string or of the trace stream name.
Minimum Acceptable Value: #define _POSIX_TRACE_NAME_MAX}
#define TRACE_SYS_MAX}
[TRC]
Maximum number of trace streams that may simultaneously exist in the system.
Minimum Acceptable Value: #define _POSIX_TRACE_SYS_MAX}
#define TRACE_USER_EVENT_MAX}
[TRC]
Maximum number of user trace event type identifiers that may simultaneously exist in a traced process, including the predefined user trace event POSIX_TRACE_UNNAMED_USER_EVENT.
Minimum Acceptable Value: #define _POSIX_TRACE_USER_EVENT_MAX}
#define TTY_NAME_MAX}
Maximum length of terminal device name.
Minimum Acceptable Value: #define _POSIX_TTY_NAME_MAX}
#define TZNAME_MAX}
Maximum number of bytes supported for the name of a timezone (not of the TZ variable).
Minimum Acceptable Value: #define _POSIX_TZNAME_MAX}*/

/* Pathname Variable Values */
/*#define FILESIZEBITS}
Minimum number of bits needed to represent, as a signed integer value, the maximum size of a regular file allowed in the specified directory.
Minimum Acceptable Value: 32
#define LINK_MAX}
Maximum number of links to a single file.
Minimum Acceptable Value: #define _POSIX_LINK_MAX}
#define MAX_CANON}
Maximum number of bytes in a terminal canonical input line.
Minimum Acceptable Value: #define _POSIX_MAX_CANON}
#define MAX_INPUT}
Minimum number of bytes for which space is available in a terminal input queue; therefore, the maximum number of bytes a conforming application may require to be typed as input before reading them.
Minimum Acceptable Value: #define _POSIX_MAX_INPUT}
#define NAME_MAX}
Maximum number of bytes in a filename (not including terminating null).
Minimum Acceptable Value: #define _POSIX_NAME_MAX}
[XSI]  Minimum Acceptable Value: #define _XOPEN_NAME_MAX}*/
/*
 * Maximum number of bytes in a pathname, 
 * including the terminating null character.
 */ 
#define PATH_MAX		256
/*
#define PIPE_BUF}
Maximum number of bytes that is guaranteed to be atomic when writing to a pipe.
Minimum Acceptable Value: #define _POSIX_PIPE_BUF}
#define POSIX_ALLOC_SIZE_MIN}
[ADV]
Minimum number of bytes of storage actually allocated for any portion of a file.
Minimum Acceptable Value: Not specified.
#define POSIX_REC_INCR_XFER_SIZE}
[ADV]
Recommended increment for file transfer sizes between the #define POSIX_REC_MIN_XFER_SIZE} and #define POSIX_REC_MAX_XFER_SIZE} values.
Minimum Acceptable Value: Not specified.
#define POSIX_REC_MAX_XFER_SIZE}
[ADV]
Maximum recommended file transfer size.
Minimum Acceptable Value: Not specified.
#define POSIX_REC_MIN_XFER_SIZE}
[ADV]
Minimum recommended file transfer size.
Minimum Acceptable Value: Not specified.
#define POSIX_REC_XFER_ALIGN}
[ADV]
Recommended file transfer buffer alignment.
Minimum Acceptable Value: Not specified.
#define SYMLINK_MAX}
Maximum number of bytes in a symbolic link.
Minimum Acceptable Value: #define _POSIX_SYMLINK_MAX}*/


/* Runtime Increasable Values */
/*#define BC_BASE_MAX}
Maximum obase values allowed by the bc utility.
Minimum Acceptable Value: #define _POSIX2_BC_BASE_MAX}
#define BC_DIM_MAX}
Maximum number of elements permitted in an array by the bc utility.
Minimum Acceptable Value: #define _POSIX2_BC_DIM_MAX}
#define BC_SCALE_MAX}
Maximum scale value allowed by the bc utility.
Minimum Acceptable Value: #define _POSIX2_BC_SCALE_MAX}
#define BC_STRING_MAX}
Maximum length of a string constant accepted by the bc utility.
Minimum Acceptable Value: #define _POSIX2_BC_STRING_MAX}
#define CHARCLASS_NAME_MAX}
Maximum number of bytes in a character class name.
Minimum Acceptable Value: #define _POSIX2_CHARCLASS_NAME_MAX}
#define COLL_WEIGHTS_MAX}
Maximum number of weights that can be assigned to an entry of the LC_COLLATE order keyword in the locale definition file; see Locale .
Minimum Acceptable Value: #define _POSIX2_COLL_WEIGHTS_MAX}
#define EXPR_NEST_MAX}
Maximum number of expressions that can be nested within parentheses by the expr utility.
Minimum Acceptable Value: #define _POSIX2_EXPR_NEST_MAX}
#define LINE_MAX}
Unless otherwise noted, the maximum length, in bytes, of a utility's input line (either standard input or another file), when the utility is described as processing text files. The length includes room for the trailing <newline>.
Minimum Acceptable Value: #define _POSIX2_LINE_MAX}
#define NGROUPS_MAX}
Maximum number of simultaneous supplementary group IDs per process.
Minimum Acceptable Value: #define _POSIX_NGROUPS_MAX}
#define RE_DUP_MAX}
Maximum number of repeated occurrences of a regular expression permitted when using the interval notation \{m,n\}; see Regular Expressions .
Minimum Acceptable Value: #define _POSIX2_RE_DUP_MAX}*/

/* Maximum Values */
/*#define _POSIX_CLOCKRES_MIN}
[TMR]
The resolution of the CLOCK_REALTIME clock, in nanoseconds.
Value: 20 000 000
[MON]  If the Monotonic Clock option is supported, the resolution of the CLOCK_MONOTONIC clock, in nanoseconds, is represented by {_POSIX_CLOCKRES_MIN}.*/

/* Minimum Values */
/*
#define _POSIX_AIO_LISTIO_MAX}
[AIO]
The number of I/O operations that can be specified in a list I/O call.
Value: 2
#define _POSIX_AIO_MAX}
[AIO]
The number of outstanding asynchronous I/O operations.
Value: 1
#define _POSIX_ARG_MAX}
Maximum length of argument to the exec functions including environment data.
Value: 4 096
#define _POSIX_CHILD_MAX}
Maximum number of simultaneous processes per real user ID.
Value: 25
#define _POSIX_DELAYTIMER_MAX}
[TMR]
The number of timer expiration overruns.
Value: 32
#define _POSIX_HOST_NAME_MAX}
Maximum length of a host name (not including the terminating null) as returned from the gethostname() function.
Value: 255
#define _POSIX_LINK_MAX}
Maximum number of links to a single file.
Value: 8
#define _POSIX_LOGIN_NAME_MAX}
The size of the storage required for a login name, in bytes, including the terminating null.
Value: 9
#define _POSIX_MAX_CANON}
Maximum number of bytes in a terminal canonical input queue.
Value: 255
#define _POSIX_MAX_INPUT}
Maximum number of bytes allowed in a terminal input queue.
Value: 255
#define _POSIX_MQ_OPEN_MAX}
[MSG]
The number of message queues that can be open for a single process.
Value: 8
#define _POSIX_MQ_PRIO_MAX}
[MSG]
The maximum number of message priorities supported by the implementation.
Value: 32
#define _POSIX_NAME_MAX}
Maximum number of bytes in a filename (not including terminating null).
Value: 14
#define _POSIX_NGROUPS_MAX}
Maximum number of simultaneous supplementary group IDs per process.
Value: 8
#define _POSIX_OPEN_MAX}
Maximum number of files that one process can have open at any one time.
Value: 20
#define _POSIX_PATH_MAX}
Maximum number of bytes in a pathname.
Value: 256
#define _POSIX_PIPE_BUF}
Maximum number of bytes that is guaranteed to be atomic when writing to a pipe.
Value: 512
#define _POSIX_RE_DUP_MAX}
The number of repeated occurrences of a BRE permitted by the regexec() and regcomp() functions when using the interval notation {\(m,n\}; see BREs Matching Multiple Characters .
Value: 255
#define _POSIX_RTSIG_MAX}
[RTS]
The number of realtime signal numbers reserved for application use.
Value: 8
#define _POSIX_SEM_NSEMS_MAX}
[SEM]
The number of semaphores that a process may have.
Value: 256
#define _POSIX_SEM_VALUE_MAX}
[SEM]
The maximum value a semaphore may have.
Value: 32 767
#define _POSIX_SIGQUEUE_MAX}
[RTS]
The number of queued signals that a process may send and have pending at the receiver(s) at any time.
Value: 32
#define _POSIX_SSIZE_MAX}
The value that can be stored in an object of type ssize_t.
Value: 32 767
#define _POSIX_STREAM_MAX}
The number of streams that one process can have open at one time.
Value: 8
#define _POSIX_SS_REPL_MAX}
[SS|TSP]
The number of replenishment operations that may be simultaneously pending for a particular sporadic server scheduler.
Value: 4
#define _POSIX_SYMLINK_MAX}
The number of bytes in a symbolic link.
Value: 255
#define _POSIX_SYMLOOP_MAX}
The number of symbolic links that can be traversed in the resolution of a pathname in the absence of a loop.
Value: 8
#define _POSIX_THREAD_DESTRUCTOR_ITERATIONS}
[THR]
The number of attempts made to destroy a thread's thread-specific data values on thread exit.
Value: 4
#define _POSIX_THREAD_KEYS_MAX}
[THR]
The number of data keys per process.
Value: 128
#define _POSIX_THREAD_THREADS_MAX}
[THR]
The number of threads per process.
Value: 64
#define _POSIX_TIMER_MAX}
[TMR]
The per-process number of timers.
Value: 32
#define _POSIX_TRACE_EVENT_NAME_MAX}
[TRC]
The length in bytes of a trace event name.
Value: 30
#define _POSIX_TRACE_NAME_MAX}
[TRC]
The length in bytes of a trace generation version string or a trace stream name.
Value: 8
#define _POSIX_TRACE_SYS_MAX}
[TRC]
The number of trace streams that may simultaneously exist in the system.
Value: 8
#define _POSIX_TRACE_USER_EVENT_MAX}
[TRC]
The number of user trace event type identifiers that may simultaneously exist in a traced process, including the predefined user trace event POSIX_TRACE_UNNAMED_USER_EVENT.
Value: 32
#define _POSIX_TTY_NAME_MAX}
The size of the storage required for a terminal device name, in bytes, including the terminating null.
Value: 9
#define _POSIX_TZNAME_MAX}
Maximum number of bytes supported for the name of a timezone (not of the TZ variable).
Value: 6
Note:
The length given by {_POSIX_TZNAME_MAX} does not include the quoting characters mentioned in Other Environment Variables .
#define _POSIX2_BC_BASE_MAX}
Maximum obase values allowed by the bc utility.
Value: 99
#define _POSIX2_BC_DIM_MAX}
Maximum number of elements permitted in an array by the bc utility.
Value: 2 048
#define _POSIX2_BC_SCALE_MAX}
Maximum scale value allowed by the bc utility.
Value: 99
#define _POSIX2_BC_STRING_MAX}
Maximum length of a string constant accepted by the bc utility.
Value: 1 000
#define _POSIX2_CHARCLASS_NAME_MAX}
Maximum number of bytes in a character class name.
Value: 14
#define _POSIX2_COLL_WEIGHTS_MAX}
Maximum number of weights that can be assigned to an entry of the LC_COLLATE order keyword in the locale definition file; see Locale .
Value: 2
#define _POSIX2_EXPR_NEST_MAX}
Maximum number of expressions that can be nested within parentheses by the expr utility.
Value: 32
#define _POSIX2_LINE_MAX}
Unless otherwise noted, the maximum length, in bytes, of a utility's input line (either standard input or another file), when the utility is described as processing text files. The length includes room for the trailing <newline>.
Value: 2 048
#define _POSIX2_RE_DUP_MAX]
Maximum number of repeated occurrences of a regular expression permitted when using the interval notation \#define m,n\}; see Regular Expressions .
Value: 255
#define _XOPEN_IOV_MAX}
[XSI]
Maximum number of iovec structures that one process has available for use with readv() or writev().
Value: 16
#define _XOPEN_NAME_MAX}
[XSI]
Maximum number of bytes in a filename (not including the terminating null).
Value: 255
#define _XOPEN_PATH_MAX}
[XSI]
Maximum number of bytes in a pathname.
Value: 1024*/

/* Numerical Limits */
/*#define CHAR_BIT}
Number of bits in a type char.
[CX]  Value: 8
#define CHAR_MAX}
Maximum value of type char.
Value: {UCHAR_MAX} or {SCHAR_MAX}
#define CHAR_MIN}
Minimum value of type char.
Value: {SCHAR_MIN} or 0
#define INT_MAX}
Maximum value of an int.
[CX]  Minimum Acceptable Value: 2 147 483 647
#define LONG_BIT}
[XSI]
Number of bits in a long.
Minimum Acceptable Value: 32
#define LONG_MAX}
Maximum value of a long.
Minimum Acceptable Value: +2 147 483 647
#define MB_LEN_MAX}
Maximum number of bytes in a character, for any supported locale.
Minimum Acceptable Value: 1
#define SCHAR_MAX}
Maximum value of type signed char.
[CX]  Value: +127
#define SHRT_MAX}
Maximum value of type short.
Minimum Acceptable Value: +32 767
#define SSIZE_MAX}
Maximum value of an object of type ssize_t.
Minimum Acceptable Value: {_POSIX_SSIZE_MAX}
#define UCHAR_MAX}
Maximum value of type unsigned char.
[CX]  Value: 255 
#define ULONG_MAX}
Maximum value of type unsigned long.
Minimum Acceptable Value: 4 294 967 295
#define USHRT_MAX}
Maximum value for a type unsigned short.
Minimum Acceptable Value: 65 535
#define WORD_BIT}
[XSI]
Number of bits in a type int.
Minimum Acceptable Value: 32
#define INT_MIN}
Minimum value of type int.
[CX]  Maximum Acceptable Value: -2 147 483 647
#define LONG_MIN}
Minimum value of type long.
Maximum Acceptable Value: -2 147 483 647
#define SCHAR_MIN}
Minimum value of type signed char.
[CX]  Value: -128
#define SHRT_MIN}
Minimum value of type short.
Maximum Acceptable Value: -32 767
#define LLONG_MIN}
Minimum value of type long long.
Maximum Acceptable Value: -9223372036854775807
#define LLONG_MAX}
Maximum value of type long long.
Minimum Acceptable Value: +9223372036854775807
#define ULLONG_MAX}
Maximum value of type unsigned long long.
Minimum Acceptable Value: 18446744073709551615*/

/* Other Invariant Values */
/*#define NL_ARGMAX}
[XSI]
Maximum value of digit in calls to the printf() and scanf() functions.
Minimum Acceptable Value: 9
#define NL_LANGMAX}
[XSI]
Maximum number of bytes in a LANG name.
Minimum Acceptable Value: 14
#define NL_MSGMAX}
[XSI]
Maximum message number.
Minimum Acceptable Value: 32 767
#define NL_NMAX}
[XSI]
Maximum number of bytes in an N-to-1 collation mapping.
Minimum Acceptable Value: No guaranteed value across all conforming implementations.
#define NL_SETMAX}
[XSI]
Maximum set number.
Minimum Acceptable Value: 255
#define NL_TEXTMAX}
[XSI]
Maximum number of bytes in a message string.
Minimum Acceptable Value: #define _POSIX2_LINE_MAX}
#define NZERO}
[XSI]
Default process priority.
Minimum Acceptable Value: 20
*/
#endif /* __LIMITS_H */
