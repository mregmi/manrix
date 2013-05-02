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
 * File: pager.h
 */

#ifndef __MRX_PAGER_H
#define __MRX_PAGER_H


//#include <mrx/pthread-internal.h>
//#include <signal.h>

//#include <asm/vm.h>

/* Object For vnode */
struct vn_object{
	int fd;
	int blk_size;
	size_t page_size;
	unsigned long va;
	unsigned long offset;
	int map_type;
	int protection;
	size_t size;
	int pager_id;	/* The pager can be default and user defined as well. */
	unsigned long noload_start, noload_end;
};

#endif /* __FSIO_PAGER_H */
