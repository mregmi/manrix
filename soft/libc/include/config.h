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

#ifndef __MANRIX_LIBC_CONFIG_H
#define __MANRIX_LIBC_CONFIG_H

/* use architecture specfic code for string */
#define USE_ARCH_STRING		1
#define HAVE_STDLIB_H		1
#define HAVE_STRING_H		1
#define HAVE_UNISTD_H		1
#define HAVE_STRING_H		1


#define _STACK_GROWS_DOWN	1	/* for this machine */

#endif /* __LIBC_CONFIG_H */
