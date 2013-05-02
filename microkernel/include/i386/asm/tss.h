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
 * File:- tss.h
 *	The Intel i386 TSS Structure.
 */

#ifndef __I386_TSS_H
#define __I386_TSS_H

#ifndef __MANIX_TYPES_H
#include <kernel/types.h>
#endif

#include <kernel/config.h>

#define TSS_AVAIL	0x09
#define TSS_BUSY	0x0B

/* Selector value for tss segment descriptor in GDT */
#define TSS_SEL		0x2B

/*
 *	i386 Task State Segment
 */
struct TSS{
	u16		back_link, back_link_d;	/* segment number of previous task, if nested */
	u32		esp0;		/* initial stack pointer ... */
	u16		ss0, ss0_d;		/* and segment for ring 0 */
	u32		esp1;		/* initial stack pointer ... */
	u16		ss1, ss1_d;		/* and segment for ring 1 */
	u32		esp2;		/* initial stack pointer ... */
	u16		ss2, ss2_d;		/* and segment for ring 2 */
	u32		cr3;		/* CR3 - page table directory physical address */
	u32		eip;
	u32		eflags;
	u32		eax;
	u32		ecx;
	u32		edx;
	u32		ebx;
	u32		esp;		/* current stack pointer */
	u32		ebp;
	u32		esi;
	u32		edi;
	u16		es, es_d;
	u16		cs, cs_d;
	u16		ss, ss_d;		/* current stack segment */
	u16		ds, ds_d;
	u16		fs, fs_d;
	u16		gs, gs_d;
	u16		ldt, ldt_d;		/* local descriptor table segment */
	u16		trace_trap;	/* trap on switch to this task */
	u16		io_bit_map_offset; 	/* offset to start of IO permission bit map */
	u32 		align[38];
};

#define DEF_TSS		{			\
				0,0,0,		\
				KERNEL_DATA,0,0,\
				0,0,		\
				0,0,		\
				0,0,		\
				0,0,		\
				0,0,		\
				0,0,		\
				0,0,		\
				0,0,		\
				0, 0,		\
				0, 0,		\
				0, 0,		\
				0, 0,		\
				0, 0,		\
				0, 0,		\
				0, 0,		\
				0, 0x8000	\
			}

extern struct TSS stss[MAX_CPU];

/*
 * Load the Task Register
 */
static __inline__ void LTR(unsigned int selector)
{
	__asm__ __volatile__ ("ltr %0": :"m"(selector));
}

#ifndef MANRIX_SMP
 #define get_cur_tss		(&stss[0])
 #define get_thr_tss(thr)	(&stss[0])
#else
 #define get_cur_tss		(&stss[current->cpu])
 #define get_thr_tss(thr)	(&stss[thr->cpu])
#endif	/* MANRIX_SMP */

#endif /* __I386_TSS_H */
