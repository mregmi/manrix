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
 * File: types.h
 *
 * This header contains a set of user defined typedefed data
 * types.
 */

#ifndef __MANIX_TYPES_H
#define __MANIX_TYPES_H

#include <sys/types.h>

#ifndef __TIME_T
#define __TIME_T
typedef long time_t;
#endif

#ifndef __SIZE_T
#define __SIZE_T
typedef unsigned int size_t;
#endif

#define NULL		((void *)0)

/* signed data types */
typedef char			s8;
typedef short			s16;
typedef int			s32;
typedef long long		s64;

/* unsigned data types */
typedef unsigned char	u8;
typedef unsigned short	u16;
typedef unsigned int	u32;
typedef unsigned long long	u64;

/* misc */
typedef u8 		BYTE;
typedef u16		WORD;
typedef u32 	DWORD;
typedef u64 	QWORD;


#endif	/* __MANIX_TYPES_H */
