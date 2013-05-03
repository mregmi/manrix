/**
 * ManRiX Microkernel
 * Copyright (C) 2004, 2005
 * 	Manish Regmi (regmi dot manish at gmail dot com)
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
 * File: apic.c
 *
 * This file handles Local Advanced programmable interrupt controller (LAPIC).
 **/
#include <kernel/config.h>
#include <asm/apic.h>
#include <asm/asm.h>
#include <asm/x86.h>

#ifdef MANRIX_LAPIC

int lapic = 0;

/*
 * The Local APIC memory is remapped to Low memory for ease
 */
unsigned long LAPIC_BASE = 0x1000;

/*
 * Check for the presence of on Chip Local APIC.
 * We can determine the presence by cpuid feature flags.
 */
static inline int apic_ispresent()
{
	struct cpu_regs regs;

	cpuid(1, &regs);
	if(regs.edx & CPU_FEAT_APIC)
		return 1;

	return 0;
}

/* Relocate the APIC address to another address */
static inline void reloc_apic(unsigned long addr, int bsp)
{
	struct cpu_regs regs;
	unsigned long base;

	base = addr >> 12;
	if(bsp)
		regs.eax = ((base << 12) & 0xFFFF) | MSR_APIC_BSP | MSR_APIC_ENABLE;
	else
		regs.eax = ((base << 12) & 0xFFFF) | MSR_APIC_ENABLE;

	regs.edx = (base >> 24) & 0xFFFF;

	wrmsr(MSR_APIC_BASE, &regs);
}

/*
 * Initialize Local APIC chip.
 * Used even in Uniprocessors for I/O APIC.
 */
int apic_init()
{
	if(!apic_ispresent())
	{
		kprintf("On Chip APIC not present.\n");
		return -1;
	}

	/* First Relocate Local APIC so that we can easily access it */
	reloc_apic(LAPIC_BASE, 1);
	lapic = 1;
}

#endif /* MANRIX_LAPIC */
