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
 * File: sysinfo.h
 * 		Gives the system information.
 */

#ifndef __SYS_SYSINFO_H
#define __SYS_SYSINFO_H

#include <sys/types.h>

/* System info */
struct sysinfo{
	int total_process;
	int total_threads;

	size_t total_mem;
	size_t used_mem;

	size_t total_ram;
	size_t used_ram;
};

/* Process info */
struct procinfo {
};

#endif
