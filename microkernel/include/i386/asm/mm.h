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

#ifndef __ASM_MM_H
#define __ASM_MM_H

#include <asm/vm_param.h>


/* Initialize The Physical memory */
extern void init_memory();

extern vm_offset_t get_next_frame();

/* Return the base address of the page frame. */
extern vm_offset_t get_frame();

extern u32 get_avail_mem();

/* Initialize the paging */
extern void init_paging();

#endif	/* __ASM_MM_H */

