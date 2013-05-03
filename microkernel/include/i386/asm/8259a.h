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
 * File:- 8259a.h
 *
 * This file handles chips like programmable interrupt controller pic(8259a).
 * Only basic features are handled here. The devices are handled in user space.
 */

#ifndef __ASM_8259A_H
#define __ASM_8259A_H

#define PIC1_COMMAND		0x20
#define PIC1_DATA		0x21

#define PIC2_COMMAND		0xA0
#define PIC2_DATA		0xA1
#define EOI			0x20	/* END OF INTERRUPT */

#define ICW1_ICW1		0x10	/* ICW1 bit 4 :- always set to 1. */
#define ICW1_ICW4		0x01	/* ICW1 bit 1 :- 1=ICW4used, 0=unused .*/

#define ICW4_8086		0x01	/* 8086 mode. */

extern void pic_init(int pic1_start, int pic2_start);
extern void enable_irq(int irq);
extern void disable_irq(int irq);

#endif /* __ASM_8259A_H */

