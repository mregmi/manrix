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
 * File:- errno.h
 */

#ifndef __ERRNO_H
#define __ERRNO_H

#define E2BIG			0x01	/* Argument list too long. */
#define EACCES			0x02	/* Permission denied. */
#define EADDRINUSE		0x03	/* Address in use. */
#define EADDRNOTAVAIL		0x04	/* Address not available. */
#define EAFNOSUPPORT		0x05	/* Address family not supported. */
#define EAGAIN			0x06	/* Resource unavailable, try again */
#define EALREADY		0x07	/* Connection already in progress. */
#define EBADF			0x08	/* Bad file descriptor. */
#define EBADMSG			0x09	/* Bad message. */
#define EBUSY			0x0a	/* Device or resource busy. */
#define ECANCELED		0x0b	/* Operation canceled.*/
#define ECHILD			0x0c	/* No child processes. */
#define ECONNABORTED		0x0d	/* Connection aborted. */
#define ECONNREFUSED		0x0e	/* Connection refused. */
#define ECONNRESET		0x0f	/* Connection reset. */
#define EDEADLK			0x10	/* Resource deadlock would occur. */
#define EDESTADDRREQ		0x12	/* Destination address required. */
#define EDOM			0x13	/* Mathematics argument out of domain of function. */
#define EDQUOT			0x14	/* Reserved. */
#define EEXIST			0x15	/* File exists. */
#define EFAULT			0x16	/* Bad address. */
#define EFBIG			0x17	/* File too large. */
#define EHOSTUNREACH		0x18	/* Host is unreachable. */
#define EIDRM			0x19	/* Identifier removed. */
#define EILSEQ			0x1a	/* Illegal byte sequence. */
#define EINPROGRESS		0x1b	/* Operation in progress. */
#define EINTR			0x1c	/* Interrupted function. */
#define EINVAL			0x1d	/* Invalid argument. */
#define EIO			0x1e	/* I/O error. */
#define EISCONN			0x1f	/* Socket is connected. */
#define EISDIR			0x20	/* Is a directory. */
#define ELOOP			0x21	/* Too many levels of symbolic links. */
#define EMFILE			0x22	/* Too many open files. */
#define EMLINK			0x23	/* Too many links. */
#define EMSGSIZE		0x24	/* Message too large. */
#define EMULTIHOP		0x25	/* Reserved. */
#define ENAMETOOLONG		0x26	/* Filename too long. */
#define ENETDOWN		0x27	/* Network is down. */
#define ENETUNREACH		0x28	/* Network unreachable. */
#define ENFILE			0x29	/* Too many files open in system. */
#define ENOBUFS			0x2a	/* No buffer space available. */
#define ENODATA			0x2b	/* No message is available on the STREAM head read queue. */
#define ENODEV			0x2c	/* No such device. */
#define ENOENT			0x2d	/* No such file or directory. */
#define ENOEXEC			0x2e	/* Executable file format error. */
#define ENOLCK			0x2f	/* No locks available. */
#define ENOLINK			0x30	/* Reserved. */
#define ENOMEM			0x31	/* Not enough space. */
#define ENOMSG			0x32	/* No message of the desired type. */
#define ENOPROTOOPT		0x33	/* Protocol not available. */
#define ENOSPC			0x34	/* No space left on device. */
#define ENOSR			0x35	/* No STREAM resources. */
#define ENOSTR			0x36	/* Not a STREAM. */
#define ENOSYS			0x37	/* Function not supported. */
#define ENOTCONN		0x38	/* The socket is not connected. */
#define ENOTDIR			0x39	/* Not a directory. */
#define ENOTEMPTY		0x3a	/* Directory not empty. */
#define ENOTSOCK		0x3b	/* Not a socket. */
#define ENOTSUP			0x3c	/* Not supported. */
#define ENOTTY			0x3d	/* Inappropriate I/O control operation. */
#define ENXIO			0x3e	/* No such device or address. */
#define EOPNOTSUPP		0x3f	/* Operation not supported on socket. */
#define EOVERFLOW		0x40	/* Value too large to be stored in data type. */
#define EPERM			0x41	/* Operation not permitted. */
#define EPIPE			0x42	/* Broken pipe. */
#define EPROTO			0x43	/* Protocol error. */
#define EPROTONOSUPPORT		0x44	/* Protocol not supported. */
#define EPROTOTYPE		0x45	/* Socket type not supported. */
#define ERANGE			0x46	/* Result too large. */
#define EROFS			0x47	/* Read-only file system. */
#define ESPIPE			0x48	/* Invalid seek. */
#define ESRCH			0x49	/* No such process. */
#define ESTALE			0x4a	/* Reserved. */
#define ETIME			0x4b	/* Stream ioctl() timeout. */
#define ETIMEDOUT		0x4c	/* Connection timed out. */
#define ETXTBSY			0x4d	/* Text file busy. */
#define EWOULDBLOCK		0x4e	/* Operation would block. */
#define EXDEV			0x4f	/* Cross-device link. */

#define EBADMAGIC		0x50

typedef int error_t;
extern error_t errno;

#ifndef __set_errno
# define __set_errno(ev) ((errno) = (ev))
#endif

#endif
