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
 * File:- i386_setup.c
 *		The I386 based initialization is done here.
 **/
#include <kernel/config.h>
#include <kernel/types.h>
#include <kernel/kernel_stdio.h>
#include <kernel/scheduling.h>

#include <asm/pmap.h>
#include <asm/descriptors.h>
#include <asm/8259a.h>
#include <asm/multiboot.h>

extern struct pmap kernel_map;

/*
 * Do the architecture Specfic Setup.
 */
void arch_setup()
{
	/*
	 * Initialize the Paging System.
	 */
	init_memory();
	pmap_pre_init();
	init_paging();

	/*
	 * First we set up our own Global descriptors
	 * and Interrupt Descriptors.
	 */
	setup_descriptors();

	/*
	 * Then, Set Up our own I386 traps (Exceptions)
	 * Handlers
	 */
	setup_traps();

	/*
	 * Remap the Programmable Interrupt Controllers
	 * just after the i386 Exceptions.
	 */
	pic_init(0x20, 0x28);

	/*
	 * Now we remove the temporary mapping.
	 */
	kernel_map.pdbr[0] = 0;
#ifdef MANRIX_SMP
	smp_init();
#endif

}

#ifdef MANRIX_SMP
void ap_boot()
{
	int cpu;

	/* initialize paging */
	init_paging();

	/* Initialize GDT */
	ap_gdt_init();

	create_idle(cpu);
}
#endif /* MANRIX_SMP */
