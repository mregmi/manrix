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

#ifndef __ASM_LIMITS_H
#define __ASM_LIMITS_H

/* Numerical Limits */
#define CHAR_BIT		8	/* Number of bits in a type char. */
#define SCHAR_MAX		127	/* Maximum value of type signed char. */
#define UCHAR_MAX		255	/* Maximum value of type unsigned char. */
#define CHAR_MAX		UCHAR_MAX	/* Maximum value of type char. */
#define SCHAR_MIN		-128	/* Minimum value of type signed char. */
#define CHAR_MIN		SCHAR_MIN	/* Minimum value of type char. */

#define UINT_MAX		((unsigned int)(~0))	/* Maximum value of type unsigned. */
#define INT_MAX			UINT_MAX	/* Maximum value of an int. */

#define LONG_BIT		32	/* Number of bits in a long. */
#define ULONG_MAX		((unsigned long)(~0L))	/* Maximum value of type unsigned long. */
#define LONG_MAX		ULONG_MAX	/* Maximum value of a long. */
#define LONG_MIN		0	/* Minimum value of type long. */

#define MB_LEN_MAX		127	/* Maximum number of bytes in a character,*/
					/* for any supported  locale. */

#define USHRT_MAX		65535	/* Maximum value for a type unsigned short. */
#define SHRT_MAX		USHRT_MAX	/* Maximum value of type short. */
#define SHRT_MIN		0	/* Minimum value of type short. */

#define SSIZE_MAX		UINT_MAX	/* Maximum value of an object of type ssize_t. */

#define WORD_BIT		LONG_BIT	/* Number of bits in a type int. */

#define INT_MIN			0		/* Minimum value of type int. */

#define LLONG_MIN		0	/* Minimum value of type long long. */
#define LLONG_MAX		0xFFFFFFFFFFFFFFFF	/* Maximum value of type long long. */
#define ULLONG_MAX		LLONG_MAX	/* Maximum value of type unsigned long long. */

/* Other Invariant Values */
/*
 * Maximum value of digit in calls to the printf() and scanf() functions.
 */ 
#define NL_ARGMAX		10

#define NL_LANGMAX		20	/* Maximum number of bytes in a LANG name. */
#define NL_MSGMAX			/* Maximum message number. */
#define NL_NMAX				/* Maximum number of bytes in an N-to-1 collation mapping. */
#define NL_SETMAX			/* Maximum set number. */
#define NL_TEXTMAX			/* Maximum number of bytes in a message string. */
#define NZERO				/* Default process priority. */

#endif /* __LIMITS_H */
