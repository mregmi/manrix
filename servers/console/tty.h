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
 * File: tty.h
 *
 */

#ifndef __CON_TTY_H
#define __CON_TTY_H

#include <termios.h>
#include <mrx/svrpool.h>

#include "vga.h"
#include "keyboard.h"

#define MAX_INPUT	512
#define VNL		0xd
#define VCR		0xa

#define KEYSTAT_EXTENDED	0x01

#define KEYSTAT_LSHIFT		0x02
#define KEYSTAT_RSHIFT		0x04
#define KEYSTAT_SHIFT		0x06

#define KEYSTAT_CAPS		0x08
#define KEYSTAT_NLOCK		0x10
#define KEYSTAT_SCROLL		0x20

#define KEYSTAT_RCTRL		0x40
#define KEYSTAT_LCTRL		0x80
#define KEYSTAT_CTRL		0xC0

#define KEYSTAT_RALT		0x100
#define KEYSTAT_LALT		0x200
#define KEYSTAT_ALT			0x300

#define FLAG_BREAK			0x01

struct termqueue{
	unsigned char queue[MAX_INPUT];
	unsigned char last_scancode;
	int end;
	unsigned long keyflag;
	int led;
	int flag;
	pthread_t thread;		/* thread waiting for input */
};

struct winattr{
	int current_x, current_y;
	int max_x, max_y;
	int attr;
	char scrbuf[DEF_HORIZ * DEF_VERT];
};

struct terminal{
	struct termios	t_termios;
	struct termqueue t_input;	/* Input Queue */
	struct termqueue t_output;	/* Output Queue */
	struct winattr	t_win;
	pid_t  pgid;
};

#define TTY		(ttys[termindex])

extern void *tty_worker(poolstruct_t pool);
extern void scr_write(const char *string);
extern void con_panic(char *str, int error);
extern int console_init();

extern void kb_intr();
extern void *kbd_thr(void *data);
extern int keyb_init();
extern unsigned char put_ascii(unsigned char scancode);
extern void read_key(unsigned char ch);
extern void put_char(unsigned char ch, struct winattr *wattr, int cur);
#endif	/* __CON_TTY_H */
