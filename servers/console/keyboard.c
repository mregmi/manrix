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
 * File: keyboard.c
 *		Generic Keyboard Driver.
 */

#include <sched.h>
#include <pthread.h>
#include <asm/syscall.h>

#include "keyboard.h"
#include "tty.h"

extern struct terminal *ttys;
extern int termindex;

static inline void termqueue_init(struct termqueue *q)
{
	q->last_scancode = 0;
	q->end = 0;
	q->keyflag = 0;
	q->led = 0;
}

/* Check errornous Scancode */
static inline int scancode_check(unsigned char scancode)
{

	switch(scancode)
	{
		case SCODE_ERR:
		case SCODE_ERR1:
//		case SCODE_BAT:
		case SCODE_ECHO:
		case SCODE_ACK:
		case SCODE_BATF:
		case SCODE_RESEND:
			return -1;

		default:
			return 0;
	}
}

/* Check if the key have effect of NUMLOCK key */
static inline int have_nlock_effect(unsigned char scancode)
{
	/* Any better way to do this ??? */
	if((scancode >= 71) && (scancode <= 83))
		return 1;
	return 0;
}

/* Check if the key have effect of caps key */
static inline int have_caps_effect(unsigned char scancode)
{
	/* Any better way to do this??? */
	if((scancode >= 16) && (scancode <= 25))
		return 1;
	else if((scancode >= 30) && (scancode <= 38))
		return 1;
	else if((scancode >= 44) && (scancode <= 50))
		return 1;

	return 0;
}

/* Check if the key have effect of shift key */
static inline int have_shift_effect(unsigned char scancode)
{
	/* Any better way to do this??? */
	if((scancode >= 2) && (scancode <= 13))
		return 1;
	else if((scancode >= 16) && (scancode <= 27))
		return 1;
	else if((scancode >= 30) && (scancode <= 41))
		return 1;
	else if((scancode >= 44) && (scancode <= 53))
		return 1;

	return 0;
}

/*
 * Check for Extended Characters and
 * for keys requiring shift or capslock
 * or num lock.
 */
static inline int check_extended(struct termqueue *q,
			unsigned char *ch)
{

	if(*ch == SCODE_EXTENDED)
		return -1;

	/* Now check for shift, capslock and numlock */
	switch(*ch)
	{
		case SCODE_LSHIFT_PRESS:
		case SCODE_RSHIFT_PRESS:
			q->keyflag |= KEYSTAT_SHIFT;
			return -1;
			break;

		case SCODE_LSHIFT_RELSE:
		case SCODE_RSHIFT_RELSE:
			q->keyflag &= ~KEYSTAT_SHIFT;
			return -1;
			break;

		case SCODE_CAPSLOCK:

			if(q->keyflag & KEYSTAT_CAPS) /* Toggle */
			{
				q->keyflag &= ~KEYSTAT_CAPS;
				q->led |= LED_CAPS_LOCK;
			}
			else
			{
				q->keyflag |= KEYSTAT_CAPS;
				q->led &= ~LED_CAPS_LOCK;
			}

			set_led(q->led);
			return -1;
			break;

		case SCODE_NUMLOCK:

			set_led(q->led);

			if(q->keyflag & KEYSTAT_NLOCK) /* Toggle */
			{
				q->keyflag &= ~KEYSTAT_NLOCK;
				q->led |= LED_NUM_LOCK;
			}
			else
			{
				q->keyflag |= KEYSTAT_NLOCK;
				q->led &= ~LED_NUM_LOCK;
			}

			return -1;
			break;

		case SCODE_SCROLLOCK:

			set_led(q->led);

			if(q->keyflag & KEYSTAT_SCROLL) /* Toggle */
			{
				q->keyflag &= ~KEYSTAT_SCROLL;
				q->led |= LED_SCROLL_LOCK;
			}
			else
			{
				q->keyflag |= KEYSTAT_SCROLL;
				q->led &= ~LED_SCROLL_LOCK;
			}

			return -1;
			break;
	}

	if(*ch & 0x80)
		return -1;

	if((q->keyflag & KEYSTAT_SHIFT) && have_shift_effect(*ch))
		*ch = SCODE(*ch);

	if((q->keyflag & KEYSTAT_CAPS) && have_caps_effect(*ch))
		*ch = SCODE(*ch);

	if((q->keyflag & KEYSTAT_NLOCK) && have_nlock_effect(*ch))
		*ch = SCODE(*ch);//todo

	return 0;
}

/* The keyboard interrupt Entry */
void kb_intr()
{
	struct termqueue *q = &TTY.t_input;
	unsigned char scancode, ascii;
	int ret;

	keyb_disable();

	scancode = read_scancode();

	ret = scancode_check(scancode);
	if(ret == -1)
		goto done;

	ret = check_extended(q, &scancode);
	if(ret == -1)
		goto done;

	/* convert to ascii and put in console */
	ascii = put_ascii(scancode);
	read_key(ascii);

done:
	q->last_scancode = scancode;
	keyb_enable();
	send_eoi();
}


/* The keyboard Interrupt handler thread entry */
/*void *kbd_thr(void *data)
{
	int ret;

	syscall_change_iopl(3);
	IRQ_init(IRQ_KBD);
	syscall_IRQ_request(IRQ_KBD, kb_intr);

	do{

		ret = syscall_IRQ_wait();

	}while(1 == 1);

	return NULL;
}
*/
/* Initialize the keyboard driver */
int keyb_init()
{
	struct termqueue *q = &TTY.t_input;

	IRQ_request(IRQ_KBD, kb_intr);
	termqueue_init(q);

	return 0;
}
