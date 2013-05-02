/*
 * ManRiX Microkernel
 * Copyright (C) 2004, 2005
 * 	Manish Regmi (regmi dot manish at gmail dot com) and
 * 	Rajesh R.C   (rajesh dot rc at gmail dot com)
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
 * File:- descriptors.h
 * 	The header contains structures and declarations for selectors, descriptors
 * and other registers for x86 hardware.
 */

/*
 * GDT layout of ManRiX OS.
 *  0 (0x00) -> Null Descriptor
 *  1 (0x08) -> kernel code area (.5 GBs)  ring 0.
 *  2 (0x10) -> kernel data area (.5 GBs) ring 0.
 *  3 (0x1b) -> USER code area (3.5 GBs)  ring 3.
 *  4 (0x23) -> USER data area (3.5 GBs) ring 3.
 *  5 (0x28) -> TSS
 *  ...
 */

#ifndef __ASM_DESCRIPTORS_H
#define __ASM_DESCRIPTORS_H

#ifndef __ASM_TYPES_H
#include <kernel/types.h>
#endif

#include <asm/tss.h>

#define MAX_GDT			0x2000		/* 8192 descriptors */
#define MAX_IDT			0x100		/* 256 interrupts */

#define KERNEL_CODE		0x08
#define KERNEL_DATA		0x10
#define USER_CODE		0x1B
#define USER_DATA		0x23

/* Descriptor Previlage levels */
#define ACS_DPL0		0x00		/* for kernel */
#define ACS_DPL1		0x20		/* for system programs (unused) */
#define ACS_DPL2		0x40		/* for system programs (unused) */
#define ACS_DPL3		0x60		/* user applications */

/* Other descriptor access values */
#define ACS_PRESENT		0x80		/* present bit */
#define ACS_CODE		0x08		/* code bit */
#define ACS_DATA		0x00		/* data bit */
#define ACS_APP			0x10

#define ACS_READ		0x02
#define ACS_WRITE		0x02

#define ACS_DESC		0x10		/* Descriptor type */

#define FLAG_GRANULARITY	0x80		/* Granularity bit */
#define FLAG_DB			0x40
#define FLAG_AVL		0x10		/* Available for use by system software */

#define DESC_TRAP		0x0F		/* Descriptor for Trap */
#define DESC_INTR		0x0E		/* Descriptor for Interrupt */
#define DESC_TSS		0x09


/* Setup a trap gate */
#define set_trap_gate(num, address, previlage) \
		setup_gate(&idt[num], (u32)address, KERNEL_CODE, previlage | ACS_PRESENT | DESC_TRAP)

/* Set up interrupt gate */
#define set_interrupt_gate(num, address) \
		setup_gate(&idt[num], (u32)address, KERNEL_CODE, ACS_DPL0 | ACS_PRESENT | DESC_INTR)



/* Structure for the Segment Descriptors */
typedef struct __DESCRIPTOR {
	u16 limit_low;
	u16 base_low;
	u8  base_mid;
	u8  access; /* type=4bit,s=1bit,dpl=2bit,present bit=1bit */
	u8  flags;  /* seg lim = 4bit,avl bit=1bit,o=1bit,d/b=1bit,granularity=1 bit */
	u8  base_high;
} DESCRIPTOR;

/* structure for system descriptors or GATES */
typedef struct {
	u16 offset_low;
	u16 selector;
	u8 reserved; /* 0 */
	u8 access;
	u16 offset_high;
} GATE;

/* Structure for Memory Management registers like gdtr, idtr etc. */
typedef struct {
	u16 dummy;
	u16 limit;
	u32 base;
} MM_REGS;


#define sidt(x)	asm("sidt %0\t":"=m"(x))
#define sgdt(x) asm("sgdt %0\t":"=m"(x))
#define str(x) asm("str %0\t":"=m"(x))

/*
 * Load GDTR register
 */
static __inline__ void LGDT(MM_REGS *gdt_p)
{
	__asm__ __volatile__ ("lgdt %0": :"m"(gdt_p->limit));
}

/*
 * Load IDTR register.
 */
static __inline__ void LIDT(MM_REGS *idt_p)
{
	__asm__ __volatile__ ("lidt %0": :"m"(idt_p->limit));
}

/*
 * Load LDTR register.
 */
static __inline__ void LLDT(unsigned int selector)
{
	__asm__ __volatile__ ("lldt %0": :"r"(selector));
}


/*
 * Fill the Descriptor Structure.
 */
static inline void setup_desc(DESCRIPTOR *gdesc, u32 base, u32 limit, u8 access, u8 flags)
{
	gdesc->limit_low = limit & 0xffff;
	gdesc->base_low = base & 0xffff;
	gdesc->base_mid = (base >> 16) & 0xff;
	gdesc->access = access ;
	gdesc->flags = flags | ((limit >> 16) & 0xF);
	gdesc->base_high = base >> 24;
}

/*
 * Fill the Gate Structure.
 */
static inline void setup_gate(GATE *gate, u32 offset, u16 selector, u8 access)
{
	gate->offset_high = offset >> 16;
	gate->offset_low = offset & 0xffff;
	gate->selector = selector;
	gate->access = access;
}

void setup_descriptors();
void setup_traps();

#endif	/* __ASM_DESCRIPTORS_H */
