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
 * File:- console.h
 *
 * This file contains declarations for console reading and
 * writing functions.
 */

#ifndef __ASM_CONSOLE_H
#define __ASM_CONSOLE_H

/* we assume the screen is 80X24 */
#define MAX_X			79
#define MAX_Y			24

/* VGA Memory TExt region Start Address */
#define SCREEN_START	0xB8000

/* Default Attribute */
#define ATTR_BW			0x07


void console_write(char *);
void console_init();

#endif  /* __ASM_CONSOLE_H */
