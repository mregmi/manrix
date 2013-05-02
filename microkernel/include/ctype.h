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
 * File:- ctype.h
 * 	These functions are as defined on Common UNIX specification,
 * and ANSI C.
 * We will not deliver full features of ctype. only few
 * will be implemented here.
 */

#ifndef __CTYPE_H
#define __CTYPE_H

#define _tolower(c)		((c) + ('a' - 'A'))
#define _toupper(c)		((c) + ('A' - 'a'))

#define toascii(c)		((c) & 0x7f)
#define isascii(c)		((c) <= 0x7f)

#define tolower(c)		((((c) < 'A') || ((c) > 'Z')) ? (c) : _tolower(c))
#define toupper(c)		((((c) < 'a') || ((c) > 'z')) ? (c) : _toupper(c))

#define isdigit(c)		((((c) >= '0') && ((c) <= '9'))? 1 : 0)

int isalnum(int);
int isalpha(int);
int iscntrl(int);
int isgraph(int);
int isupper(int);
int ispunct(int);
int isspace(int);
int isxdigit(int);

#endif		/* __CTYPE_H */
