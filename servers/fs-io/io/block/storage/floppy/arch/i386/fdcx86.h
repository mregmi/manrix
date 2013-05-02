/*
 * FS and I/O manager for ManRiX OS
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
 * File: fdcx86.h
 * 	Architecture dependent FDC Declarations and macros.
 */

#ifndef __ASM_FDC_H
#define __ASM_FDC_H

#include <stdint.h>

#include <asm/asm.h>

#define PRIMARY_FDC		0x3f0
#define SECONDARY_FDC		0x370

#define IRQ_FDC			6
#define FDC_DMA			2

#define PIC1_COMMAND	0x20
#define PIC1_DATA		0x21

#define PIC2_COMMAND	0xA0
#define PIC2_DATA		0xA1

static inline uint16_t get_port(unsigned char addr)
{
	uint16_t port = PRIMARY_FDC;	/* default port */

	port = PRIMARY_FDC + addr;

	return port;
}


/* Read a Byte from FDC controller */
#define READ_REG_BYTE(addr)	(inportb(get_port(addr)))

/* Write a Byte to FDC controller */
#define WRITE_REG_BYTE(addr, value)	(outportb(value, get_port(addr)))

/* Read a Word from FDC controller */
#define READ_REG_WORD(addr)	(inportw(get_port(addr)))

/* Write a Word to FDC controller */
#define WRITE_REG_WORD(addr, value)	(outportw(value, get_port(addr)))

/* Read a Dword from FDC controller */
#define READ_REG_DWORD(addr)	(inportl(get_port(addr)))

/* Write a Dword to FDC controller */
#define WRITE_REG_DWORD(addr, value)	(outportw(value, get_port(addr)))

/* Read a block of Data from FDC controller */
#define READ_DATA(addr, data, len)	(insw(data, len, get_port(addr)))

/* Write a Block of Data to FDC controller */
#define WRITE_DATA(addr, data, len)	(outsw(data, len, get_port(addr)))

static inline void IRQ_init(int irq)
{
	if(irq < 8)
		outportb(inportb(PIC1_DATA) & ~(1 << irq), PIC1_DATA);
	else
		outportb(inportb(PIC2_DATA) & ~(1 << (irq-8)), PIC2_DATA);

}

static inline void finish_intr()
{
	outportb(0x20, 0x20);
	outportb(0x20, 0xA0);
}


/*
 *	DMA Chip Port Address for AT PC.
 *     DMA Channel   Page    Address  Count
 *     ------------------------------------
 *         0         87h       0h      1h
 *         1         83h       2h      3h
 *         2         81h       4h      5h
 *         3         82h       6h      7h
 *         4         8Fh      C0h     C2h
 *         5         8Bh      C4h     C6h
 *         6         89h      C8h     CAh
 *         7         8Ah      CCh     CEh
 *
 *     Mask Register Set/reset mode :- 0x0A	(master)
 *     Mode Register :- 0x0B
 *     Flip/Flop :- 0x0C
 */
static inline void dma_init(void *addr, int size, int type)
{
	unsigned char page, off_low, off_high, mode, cnt_low, cnt_high;

	page = (unsigned char)((unsigned long)addr >> 16);				/* A16 - A23 (BUS) */
	off_low = (unsigned char)((unsigned long)addr & 0xFF);			/* A0 - A7 */
	off_high = (unsigned char)(((unsigned long)addr >> 8) & 0xFF);	/* A8 - A15 */

	size = size - 1;
	cnt_low = (unsigned char)(size & 0xFF);
	cnt_high = (unsigned char)((size >> 8) & 0xFF);

	/*
	 * Disable Interrupts;
	 * Make sure we are the only one messing up with DMA chip.
	 */
	asm ("cli");

	/* Select DMA channel */
	outportb(FDC_DMA|4, 0x0A);

	/* Clear F/L Flip/Flop */
	outportb(0, 0x0C);

	/* Select mode */
	mode = FDC_DMA;
	if(type & BLK_READ)
		mode |= 0x48;	/* Read, Single mode */
	else
		mode |= 0x44;	/* Write, Single Mode */
	outportb(mode, 0x0B);

	/* Set Page */
	outportb(page, 0x81);

	/* Set offset */
	outportb(off_low, 0x04);
	outportb(off_high, 0x04);

	/* Transfer Count (bytes) */
	outportb(cnt_low, 0x05);
	outportb(cnt_high, 0x05);

	/* Clear the Mask */
	outportb(FDC_DMA, 0x05);

	asm ("sti");
}

#endif	/* __ASM_FDC_H  */
