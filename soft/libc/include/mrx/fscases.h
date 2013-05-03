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
 * File: fscases.h
 */

#ifndef __FSCASES_H
#define __FSCASES_H


#define ID_FSIO_MNGR		2

#define FS_DATA_REQ		0
#define FS_OPEN			1
#define FS_CLOSE		2
#define FS_READ			3
#define FS_WRITE		4
#define FS_CHOWN		5
#define FS_EXEC			6
#define GET_IOSPACE		7
#define FS_ACCESS		8
#define FS_CHDIR		9
#define FS_CHMOD		10
#define FS_STAT			11
#define FS_FSTAT		12
#define FS_GET_CWD		13


#define SVR_OPEN		1
#define SVR_CLOSE		2
#define SVR_READ		3
#define SVR_WRITE		4

#endif /* __FSCASES_H */
