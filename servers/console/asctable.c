/*
 * Console manager for XXX OS
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
 * File: asctable.c
 *		Scancode To Extended Ascii is done here.
 */

#include "tty.h"

extern struct terminal *ttys;
extern int termindex;

unsigned char table_code1[] =
{
	0x1B, '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'0', '-', '=', 0, 0, 'q', 'w', 'e', 'r', 't',
	'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a',
	's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',
	'`', 0, 0, 'z', 'x', 'c', 'v', 'b', 'n', 'm',
	',', '.', '/', 0, '*', 0, ' ', 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,	0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,0x1B, '!',
	'@', '#', '$', '%', '^', '&', '*', '(',	')', '_',
	'+', 0, 0, 'Q', 'W', 'E', 'R', 'T',	'Y', 'U',
	'I', 'O', 'P', '{', '}', 0, 0, 'A', 'S', 'D', 'F',
	'G', 'H', 'J', 'K', 'L', ':', '|','"', 0, 0,
	'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, '7', '8', '9', '-',
	'4', '5', '6', '+', '1', '2', '3', '0', '.', 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0
};

unsigned char put_ascii(unsigned char scancode)
{
	unsigned char ascii;
	struct winattr *wattr = &TTY.t_win;

	ascii = table_code1[scancode-1];
	put_char(ascii, wattr, 1);

	return ascii;
}

