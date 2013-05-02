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
 * File: console.c
 *
 * This file contains definations for console reading and
 * writing functions.
 * This will be only used while booting (i.e till graphics
 * driver is loaded ). Later screen functions will be handled
 * by syscall, keyboard driver and graphics driver.
 */

#include <string.h>

#include <kernel/types.h>
#include <kernel/vm_param.h>
#include <kernel/scheduling.h>
#include <asm/console.h>
#include <asm/asm.h>
#include <kernel/info.h>

/* grub writes above that screen. we wont erase that */
int current_x = 0, current_y = 9;

u8 *VGA = (u8 *) SCREEN_START;
int attr = ATTR_BW;
extern struct sysinfo sysinfo;

/* scroll the screen one char up */
static void scroll()
{
	int i=0;
	u8 *up = (u8 *) SCREEN_START;
	u8 *down = (u8 *) (SCREEN_START + 160);

	memset(up, 0, 160);
	while(i < 25)
	{
		memcpy(up, down, 160);
		up += 160;
		down += 160;
		i++;
	}

	memset((void *)(SCREEN_START + (160 * 24)), 0, 160);
	current_x= 0;
	current_y = 24;

}

/* update the cursor position */
static void update_cursor(int x, int y)
{
	u16 position = (y * 80) + x;

	/* cursor LOW port to vga INDEX register */
	outportb(0x0E, 0x3D4);
	outportb((u8)((position >> 8) & 0xFF), 0x3D5);

	/* cursor HIGH port to vga INDEX register */
	outportb(0x0F, 0x3D4);
	outportb((u8)(position & 0xFF), 0x3D5);

}

/*
 * Put a character to the screen.
 */
static void put_char(char ch)
{

	/* check if it is a control character */
	switch(ch)
	{
		case 0xb: current_y += 5; goto end;
		case 0x9: current_x += 5;goto end;
		case 0xa: current_y += 1;current_x = 0;goto end; /* Unix behaviour */
		case 0xd: current_x = 0;goto end;
	}

	/* First check if we are writing in correct place or not */
	if(current_x > MAX_X)
	{
		current_x = 0;
		current_y += 1;
	}
	if(current_y > MAX_Y)
		scroll();

	VGA[(current_y * 160) + (current_x * 2)] = ch;
	VGA[(current_y * 160) + (current_x * 2 + 1)] = attr;

	current_x++;
end: ; /* do nothing */
}

/*
 * Write the String to the Console.
 */
void console_write(char *string)
{
	while(*string != '\0')
	{
		put_char(*string);
		string++;
	}
	update_cursor(current_x, current_y);
}
