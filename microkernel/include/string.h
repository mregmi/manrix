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
 * File:- string.h
 * 	The header is as defined on Common UNIX specification,
 * and ANSI C.
 * This header contains a set of macros and functions for handling
 * strings.
 */

#ifndef __STRING_H
#define __STRING_H

#ifndef __SYS_TYPES_H
#include <sys/types.h>
#endif

/* We need really fast string handling functions. They are written as
 * inline assembly in arch specfic code.
 */

#include <asm/string.h>

#ifdef NO_ARCH_STRING

/* For those which does not have arch specfic code.
 * will be Implemented later. when we port to such arch.
 */

static inline int strlen(const char *str)
{
	register int __ret = 0;
	while(*str++)
		__ret++;
	return __ret;
}

/* This cannot be inline and this is stupid logic. */
static inline char *strrev(char *str)
{
	register char *__str;
	register int len;
	len = strlen(str);
	while(--len)
		*__str++ = str[len];
	*__str = '\0';
	return __str;
}

#endif /* NO_ARCH_STRING */

#endif /* __STRING_H */

