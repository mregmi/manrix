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
 * File: keyboard.h
 *
 */

#ifndef __CON_KEYBOARD_H
#define __CON_KEYBOARD_H

#include <asm/asm.h>

#define IRQ_KBD			0x01	/* IRQ line for Keyboard */

#define PORT_PPI_A		0x60
#define PORT_PPI_B		0x61

#define PORT_KEYBD_DATA	PORT_PPI_A
#define PORT_KEYBD_ACK	PORT_PPI_B
#define PORT_KEYBD_CTRL	0x64

#define PIC1_COMMAND	0x20
#define PIC1_DATA		0x21

#define PIC2_COMMAND	0xA0
#define PIC2_DATA		0xA1

#define KEBD_CMD_LED		0xED	/* set kerboard leds */
#define KEBD_CMD_DIAG		0xEE	/* Diagnostic */
#define KEBD_CMD_SCODE		0xF0	/* Select scancode set */
#define KEBD_CMD_IDENTIFY	0xF2	/* Identify Keyboard */
#define KEBD_CMD_TYPMATIC	0xF3	/* Typematic Rate Programming */

/* Led masks */
#define LED_SCROLL_LOCK		1
#define LED_NUM_LOCK		2
#define LED_CAPS_LOCK		4

/* Various Scancodes */
#define SCODE_ERR			0x00	/* Keyboard Error */
#define SCODE_BAT			0xAA	/* Basic Assurance Test */
//#define SCODE_IVAL			0xAB	/*  */
//#define SCODE_IVAL			0x41	/*  */
#define SCODE_ECHO			0xEE	/* Result of Echo Command */
#define SCODE_ACK			0xFA	/* Acknowledge of CMD */
#define SCODE_BATF			0xFC	/* Basic Assurance Test Failed */
#define SCODE_RESEND		0xFE	/* Resend Data */
#define SCODE_ERR1			0xFF	/* Keyboard error */

#define SCODE_NL		0xA
#define SCODE_EXTENDED		0xE0	/* Extended key pressed */

#define SCODE_LSHIFT_PRESS	42
#define SCODE_LSHIFT_RELSE	(42 | 0x80)

#define SCODE_RSHIFT_PRESS	54
#define SCODE_RSHIFT_RELSE	(54 | 0x80)

#define SCODE_NUMLOCK		69
#define SCODE_CAPSLOCK		58
#define SCODE_SCROLLOCK		70

/*
 * We create our own scancode for keys requiring shift or capslock
 * or num lock. It starts from 100(is it safe?).
 */

#define SCODE(n)			(SCODE_BASE + n)

#define SCODE_BASE			128
#define SCODE_EXCLA			SCODE(1)	/* '!' */
#define SCODE_AT			SCODE(2)	/* '@' */
#define SCODE_HASH			SCODE(3)	/* '#' */
#define SCODE_DOLLAR		SCODE(4)	/* '$' */
#define SCODE_PERCENT		SCODE(5)	/* '%' */

#define send_eoi()			outportb(0x20, 0x20)

/* Read the scancode from the keyboard */
#define read_scancode()	inportb(PORT_KEYBD_DATA)

/* Read the status port. */
#define read_status()	inportb(PORT_KEYBD_CTRL)

static inline void check_kbd()
{
	int loop;
	for(loop = 0; loop < 100000; loop++)
		if(!(read_status() & 2))
			break;
}

/* Send the command to the keyboard */
static inline void send_command(int cmd)
{
	check_kbd();
	outportb(cmd, PORT_KEYBD_CTRL);
}

/* Disable the keyboard */
static inline void keyb_disable()
{
	unsigned char data;

	data = inportb(PORT_KEYBD_ACK);
	data |= 0x80;
	outportb(data, PORT_KEYBD_ACK);
}

/* Enable the keyboard */
static inline void keyb_enable()
{
	unsigned char data;

	data = inportb(PORT_KEYBD_ACK);
	data &= ~0x80;
	outportb(data, PORT_KEYBD_ACK);
}

/* set the keyboard leds */
static inline void set_led(unsigned char mask)
{
//	unsigned char data;

	//data = inportb();
	check_kbd();
	outportb(KEBD_CMD_LED, PORT_KEYBD_CTRL);
	outportb(mask, PORT_KEYBD_CTRL);
}


static inline void IRQ_init(int irq)
{
/*	if(irq < 8)
		outportb(inportb(PIC1_DATA) & ~(1 << irq), PIC1_DATA);
	else
		outportb(inportb(PIC2_DATA) & ~(1 << (irq-8)) ,PIC2_DATA);*/
}

#endif /* __CON_KEYBOARD_H */
