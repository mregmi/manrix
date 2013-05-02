/*
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
 */
/*
 * File: kernel_stdio.h
 *
 * This file contains declarations for standard input and
 * output functions.
 */

#ifndef __MANIX_KERNEL_STDIO_H
#define __MANIX_KERNEL_STDIO_H

#ifndef __STDARG_H
#include <stdarg.h>
#endif

#include <kernel/config.h>

#define BUFSIZE			512
#define LOGBUFSIZE		4096

/* flags for vsprintf */
/*
#define LEFT			1
#define RIGHT			2
#define SPACE			4
#define HASH			8
#define ZERO			16
*/
#define SIGNED			32
#define CASE			64

int kprintf(const char *, ...);
int vsprintf(char *, const char *, va_list);

void panic(const char *fmt, ...);

#ifdef MANRIX_DEBUG
static inline void DEBUG(const char *string)
{
 	char buf[512];
	va_list ap;

	va_start(ap, string);
	vsprintf(buf, string, ap);
 	kprintf("%s: %d: %s", __FILE__, __LINE__, buf);
	va_end(ap);
}

static inline void INFO(const char *string)
{
 	char buf[512];
	va_list ap;

	va_start(ap, string);
	vsprintf(buf, string, ap);
 	kprintf("%s", buf);
	va_end(ap);
}

#else
 #define DEBUG 			do{}while(0)
 #define INFO 			do{}while(0)
#endif /* MANRIX_DEBUG */

#endif	/* __MANIX_KERNEL_STDIO_H */
