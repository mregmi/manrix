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
 * File:- stdarg.h
 * 	The header is as defined on Common UNIX specification,
 * and ANSI C.
 * This header contains a set of macros which allows portable
 * functions that accept variable argument lists to be written.
 */

 #ifndef __STDARG_H
 #define __STDARG_H

 typedef char* va_list;

 #define __size(s) sizeof(s)

 /* The va_start() macro is invoked to initialise ap to the beginning
  * of the list before any calls to va_arg().
  */
 #define va_start(ap, argN) \
 	((ap = (va_list)(&argN)+__size(argN)))

 /* End ap */
 #define va_end(ap) ((void)0)

 /* This macro will return the next argument in the list pointed to by
  * ap. Each invocation of va_arg() modifies ap so that the values of
  * successive arguments are returned in turn.
  */
 #define va_arg(ap, type) \
 	(*((type *)(ap))++)

 #endif /* __STDARG_H */
