/*
 * Misc I/O manager for ManRiX OS
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
 * File: x86.h
 * 		X86 processor services and identification declaration.
 */

#ifndef __X86_H
#define __X86_H

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

/* Cpu Information */
struct cpu_info{
	char vendor_id[13];		/* vendor id: GenuineIntel in intels. */
	unsigned long serial[3];
	unsigned long signature;	/* signature indentifying processor */
	unsigned long misc_info;	/* misc info */
	unsigned long features_low, features_high;
	char brand_str[70];

	/* TLB and Cache Info */
	unsigned char tlb_data, tlb_code;
	unsigned char cache_trace;
	unsigned char cache_L1_code, cache_L1_data;
	unsigned char cache_L2;
	unsigned char cache_L3;

	char *tlb_data_str, *tlb_code_str;
	char *trace_str;
	char *L1_code_str, *L1_data_str;
	char *L2_str;
	char *L3_str;
};

/* the registers used by cpuid */
struct cpu_regs{
	unsigned int eax;
	unsigned int ebx;
	unsigned int ecx;
	unsigned int edx;
};

/* Get cpu identification */
static inline void cpuid(unsigned int val, struct cpu_regs *regs)
{
	asm volatile(
		"cpuid \n\t"
		:"=a"(regs->eax), "=b"(regs->ebx), "=c"(regs->ecx), "=d"(regs->edx)
		:"0"(val)
		);
}

#endif /* __X86_H */
