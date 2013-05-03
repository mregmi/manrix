/**
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
 **/
/**
 * File:- x86.h
 * 	The x86 processor primitives
 **/


#ifndef __ASM_X86_H
#define __ASM_X86_H

#ifndef __MANRIX_TYPES_H
#include <kernel/types.h>
#endif

/* X86 eflags mask */
#define EFLAG			0x00000002	/* always set */
#define EFLAG_CARRY		0x00000001	/* carry bit */
#define EFLAG_PARITY	0x00000004	/* parity bit */
#define EFLAG_AUX		0x00000010	/* auxillary carry bit */
#define EFLAG_ZERO		0x00000040	/* zero bit */
#define EFLAG_SIGN		0x00000080	/* sign bit */
#define EFLAG_TRAP		0x00000100	/* trap bit */
#define EFLAG_INT		0x00000200	/* interrupt flag */
#define EFLAG_DIR		0x00000400	/* direction flag */
#define EFLAG_OFLOW		0x00000800	/* overflow bit */

#define EFLAG_IOPL0		0x00000000	/* I/O previlage level 0 */
#define EFLAG_IOPL1		0x00001000	/* I/O previlage level 1 */
#define EFLAG_IOPL2		0x00002000	/* I/O previlage level 2 */
#define EFLAG_IOPL3		0x00003000	/* I/O previlage level 3 */

#define EFLAG_NT		0x00004000	/* Nested task bit */
#define EFLAG_RF		0x00010000	/* Resume bit */
#define EFLAG_VM		0x00020000	/* virtual mode (v86 mode) */
#define EFLAG_AC		0x00040000	/* Alignment check bit */
#define EFLAG_VIF		0x00080000	/* Virtual interrupt flag */
#define EFLAG_VIP		0x00100000	/* Virtual interrupt pending */
#define EFLAG_ID		0x00200000	/* Identification flag (supports cpuid if set) */

#define IOPL_SHIFT		12

#define BIOS_ROM_START	0xE0000
#define BIOS_ROM_END	0xFFFFF

/* CPU feature flags */
#define CPU_FEAT_APIC	0x0100

/* MSRs */
#define MSR_APIC_BASE	0x1B	/* This MSR holds the base address of LAPIC */
#define MSR_APIC_BSP	0x10	/* BSP bit in MSR */
#define MSR_APIC_ENABLE	0x80	/* APIC enable bit in MSR */

/* the registers used by cpuid */
struct cpu_regs{
	unsigned int eax;
	unsigned int ebx;
	unsigned int ecx;
	unsigned int edx;
};

/* Write to MSR (P5+ only) */
static inline void wrmsr(int reg, struct cpu_regs *regs)
{
	asm volatile(
			"wrmsr"
			:
			:"c"(reg), "a"(regs->eax), "d"(regs->edx)
			);
}

/* Read from MSR (P5+ only) */
static inline void rdmsr(int reg, struct cpu_regs *regs)
{
	asm volatile(
			"rdmsr"
			:"=a"(regs->eax), "=d"(regs->edx)
			:"c"(reg)
			);
}


/* Get cpu identification */
static inline void cpuid(unsigned int val, struct cpu_regs *regs)
{
	asm volatile(
		"cpuid \n\t"
		:"=a"(regs->eax), "=b"(regs->ebx), "=c"(regs->ecx), "=d"(regs->edx)
		:"0"(val)
		);
}

/*
 * Read CR2 register
 */
static inline unsigned int read_cr2()
{
	register unsigned int cr2;
	__asm__ volatile(
		"movl %%cr2, %0"
		:"=r"(cr2)
		);
	return cr2;
}

/*
 * Read CR3 register
 */
static inline unsigned int read_cr3()
{
	register unsigned int cr3;
	__asm__ volatile(
		"movl %%cr3, %0"
		:"=r"(cr3)
		);
	return cr3;
}

/* Write to cr3 register */
static inline void write_cr3(unsigned int cr3)
{
	__asm__ volatile(
		"movl %0, %%cr3"
		:
		:"r"(cr3)
		);
}

/* Invalidate a TLB Entry */
static inline void invlpg(unsigned long *address)
{
	if(address)
	{
		__asm__ volatile(
		"invlpg %0"
		:
		:"m"(address)
		:"eax"
		);

		//		return;
	}

	asm volatile(
	"movl %%CR3, %%eax\n\t"
	"movl %%eax, %%cr3\n\t"
	:
	:
	:"eax"
	);
}


#endif	/* __ASM_X86_H */
