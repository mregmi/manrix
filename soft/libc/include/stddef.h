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
/*	file: stddef.h
 * 		standard type definitions.
 */


#ifndef __STDDEF_H
#define __STDDEF_H

#ifndef NULL
 #define NULL	((void *) 0)
#endif

typedef signed int ptrdiff_t;

#ifndef __WCHAR_T
#define __WCHAR_T
typedef int wchar_t;
#endif /* __WCHAR_T */

#ifndef __SIZE_T
#define __SIZE_T
typedef unsigned int size_t;
#endif

#define EXIT_FAILURE		(!0)	/*  Unsuccessful termination for exit() */
					/* evaluates to a non-zero value. */
#define EXIT_SUCCESS		0	/* Successful termination for exit(); */
					/* evaluates to 0. */

#endif  /* __STDDEF_H  */
