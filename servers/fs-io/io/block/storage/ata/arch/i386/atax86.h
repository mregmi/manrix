/*
 * FS and I/O manager for XXX OS
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
 *
 */
 /*
 * File: ata.h
 * 			Architecture dependent AT Atachment device Declarations and macros.
 */

#ifndef __ASM_ATA_H
#define __ASM_ATA_H

#include <stdint.h>

#include <asm/asm.h>

#define PRIMARY_ATA		0x1f0
#define SECONDARY_ATA	0x170

#define IRQ_PRIMARY_IDE		14
#define IRQ_SECONDARY_IDE	15

#define PIC1_COMMAND	0x20
#define PIC1_DATA		0x21

#define PIC2_COMMAND	0xA0
#define PIC2_DATA		0xA1

static inline uint16_t get_port(unsigned char disk, unsigned char addr)
{
	uint16_t port = 0x1f0;	/* default port */

	if(disk & 2)
		port = SECONDARY_ATA + addr;
	else
		port = PRIMARY_ATA + addr;

	return port;
}

/* Read from ATA controller */
#define READ_REG(disk, addr)	(inportb(get_port(disk, addr)))

/* Write to ATA controller */
#define WRITE_REG(disk, addr, value)	(outportb(value, get_port(disk, addr)))

#define READ_REG_WORD(disk, addr)	(inportw(get_port(disk, addr)))

#define WRITE_REG_WORD(disk, addr, value)	(outportw(value, get_port(disk, addr)))

#define READ_REG_DWORD(disk, addr)	(inportl(get_port(disk, addr)))

#define WRITE_REG_DWORD(disk, addr, value)	(outportw(value, get_port(disk, addr)))

#define READ_DATA(disk, addr, data, len)	(insw(data, len, get_port(disk, addr)))
#define WRITE_DATA(disk, addr, data, len)	(outsw(data, len, get_port(disk, addr)))

#define WRITE_DEVCTRL(data)		outportb(data, 0x3f6)

static inline void IRQ_init(int irq)
{
	if(irq < 8)
		outportb(inportb(PIC1_DATA) & ~(1 << irq), PIC1_DATA);
	else
		outportb(inportb(PIC2_DATA) & ~(1 << (irq-8)), PIC2_DATA);

}

static inline void finish_intr()
{
//	outportb(0x20, 0x20);
	outportb(0x20, 0xA0);
}

#endif	/* __ASM_ATA_H  */
