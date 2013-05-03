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
 * File: console.c
 *		The text screen is handling routines.
 */

#include <sched.h>
#include <pthread.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "tty.h"
#include "vga.h"

extern struct terminal *ttys;
extern int termindex;
char *vga = NULL;


int console_init()
{
	struct winattr *wattr = &TTY.t_win;


	wattr->current_x = wattr->current_y = 8;

	vga = vga_init(vga, &wattr->max_x, &wattr->max_y, &wattr->attr);

	if(!vga)
		return -1;
	return 0;
}

/* Scroll the screen */
void scroll(struct winattr *win)
{
	int i = 0, xpix = win->max_x * 2;
	char *up = (char *)vga;
	char *down = (char *)(vga + xpix);

	memset(up, 0, xpix);
	while(i < win->max_y)
	{
		memmove(up, down, xpix);
		up += xpix;
		down += xpix;
		i++;
	}

	memset((vga + ((win->max_x)* 2 * (win->max_y-1))), 0, xpix);
	win->current_x = 0;
	win->current_y = win->max_y-1;
}


static inline void put_char_arch(struct winattr *w, unsigned char ch)
{
	vga[(w->current_y * w->max_x * 2) + (w->current_x * 2)] = ch;
	vga[(w->current_y * w->max_x * 2) + (w->current_x * 2 + 1)] = w->attr;
}

void put_char(unsigned char ch, struct winattr *wattr, int cur)
{
	if(ch == VNL)
	{
		wattr->current_x = 0;
		wattr->current_y += 1;
	}
	else if(ch == VCR)
	{
		wattr->current_y += 1;
		wattr->current_x = 0;
	}


	/* First check if we are writing in correct place or not */
	if(wattr->current_x >= wattr->max_x)
	{
		wattr->current_x = 0;
		wattr->current_y += 1;
	}
	if(wattr->current_y >= wattr->max_y)
		scroll(wattr);
	
	/* Update the cursor */
	if(cur)
		update_cursor(wattr->current_x+1, wattr->current_y);
		
	/* check if it is a control character */
	if(iscntrl(ch))
		return;

	put_char_arch(wattr, ch);

	wattr->current_x++;
}

void scr_write(const char *string)
{
	struct terminal *term;
	struct winattr *wattr;
	
	term = &TTY;
	wattr = &term->t_win;

	while(*string != '\0')
	{
		put_char(*string, wattr, 0);
		string++;
	}
	update_cursor(wattr->current_x+1, wattr->current_y);
}

#define BUFSIZE			512

/* flags for vsprintf */
/*#define LEFT			1
#define RIGHT			2
#define SPACE			4
#define HASH			8
#define ZERO			16*/
#define SIGNED			32
#define CASE			64
char buf[BUFSIZE] = {[0 ... BUFSIZE-1] = 0};


/* used to print in kernel mode */
int con_printf(const char *format, ...)
{
	va_list ap;
	int ret;
	
	va_start(ap, format);
	ret = vsprintf(buf, format, ap);

	va_end(ap);

	scr_write(buf);

	return ret;
}
