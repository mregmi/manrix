/*
 * Console manager for ManRiX OS
 * Copyright (C) 2004, 2005	Manish Regmi (regmi dot manish at gmail dot com)
 *				            Rajesh R.C (rajee5283 at hotmail dot com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/*
 * File: vga.h
 *
 */

#ifndef __CON_VGA_H
#define __CON_VGA_H

#include <sys/mman.h>
#include <asm/asm.h>

#define VGA_LIN_ADDR		0xB8000	/* Color VGA linear FB address */
#define MVGA_LIN_ADDR		0xB0000	/* Monochrome VGA Linear FB address */

#define DEF_HORIZ			80
#define DEF_VERT			25

/* Colour attributes */
#define ATTR_BLINK			0x80

#define BACKGROUND_COL(col)	(col << 4)

#define BLACK				0
#define BLUE				1
#define GREEN				2
#define CYAN				3
#define RED					4
#define MAGENTA				5
#define BROWN				6
#define WHITE				7
#define DARK_GREY			8
#define BRIGHT_BLUE			9
#define BRIGHT_GREEN		10
#define BRIGHT_CYAN			11
#define PINK				12
#define BRIGHT_MAGENTA		13
#define YELLOW				14
#define BRIGHT_WHITE		15

#define COLOR(blk, bck, forg)	(blk | BACKGROUND_COL(bck) | forg)


#define CRTC_ADDR_PORT		0x3D4
#define CRTC_DATA_PORT		0x3D5

/* update the cursor position */
static inline void update_cursor(int x, int y)
{
	unsigned short position = (y * 80) + x;

	outportb(0x0E, CRTC_ADDR_PORT);
	outportb((unsigned char)((position >> 8) & 0xFF), CRTC_DATA_PORT);

	outportb(0x0F, CRTC_ADDR_PORT);
	outportb((unsigned char)(position & 0xFF), CRTC_DATA_PORT);

}

static inline char *vga_init(char *vga, int *x, int *y, int *attr)
{
	int size = (DEF_HORIZ * DEF_VERT);
	vga = mmap((void *)VGA_LIN_ADDR, size, PROT_READ|PROT_WRITE,
			MAP_ANON|MAP_SHARED, -1, 0);

	*x = DEF_HORIZ;
	*y = DEF_VERT;

	*attr = COLOR(0, BLUE, RED);

	return vga;
}


#endif /* __CON_VGA_H */
