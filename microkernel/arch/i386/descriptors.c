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
 * File:- descriptors.h
 * 	We setup our own Descriptors here.
 */

/*
 * Memory layout of OUR-OS.
 *  0 (0x00) -> Null Descriptor
 *  1 (0x08) -> kernel code area (4 GBs)  priority 0.
 *  2 (0x10) -> kernel data area (4 GBs) priority 0.
 *  3 (0x1B) -> User data
 *  4 (0x23)
 *  5... () -> Used for TSS and LDTs.
 *  ...
 */

#include <kernel/vm_param.h>
#include <kernel/kernel_stdio.h>
#include <kernel/types.h>
#include <kernel/scheduling.h>
#include <asm/descriptors.h>
#include <asm/tss.h>
#include <asm/asm.h>
#include <asm/pmap.h>

extern int N_PROCS;

/* Per CPU TSS */
struct TSS stss[MAX_CPU] __attribute__ ((aligned (4096)));

DESCRIPTOR *gdt = NULL;

/*
 * Set up our Own Global Descriptor Table.
 */
static void setup_GDT()
{

	/* Setup NULL descriptor */
	setup_desc(&gdt[0],0,0,0,0);

	/* Setup descriptor for Kernel Code starts at 3.5g and ends at 4g*/
	setup_desc(&gdt[1], 0xE0000000, 0x1FFFF,/*,0,0xFFFFFFFF,*/
		 ACS_PRESENT | ACS_CODE | ACS_APP ,
		 FLAG_GRANULARITY | FLAG_DB );

	/* Setup descriptor for Kernel Data */
	setup_desc(&gdt[2], 0xE0000000, 0x1FFFF,/*,0,0xFFFFFFFF,*/
		ACS_PRESENT | ACS_DATA | ACS_WRITE |ACS_APP,
		FLAG_GRANULARITY | FLAG_DB);

	/* Setup descriptor for USER Code starts at 3.5g and ends at 4g*/
	setup_desc(&gdt[3], 0x0, 0xDFFFF,
			 ACS_PRESENT | ACS_CODE |ACS_APP |ACS_DPL3,
			 FLAG_GRANULARITY | FLAG_DB );

		/* Setup descriptor for USER Data */
	setup_desc(&gdt[4], 0x0, 0xDFFFF,
			 ACS_PRESENT | ACS_DATA | ACS_WRITE |ACS_APP |ACS_DPL3,
			 FLAG_GRANULARITY | FLAG_DB );

	setup_desc(&gdt[5], (vm_offset_t)linear_to_virtual(&stss[0]), sizeof(struct TSS),
			ACS_PRESENT | DESC_TSS,
			FLAG_AVL);
}


/* jump to new segment */
static inline void goto_segment(u16 code, u16 data)
{
	__asm__ __volatile__ (
				"ljmp %0, $1f\t\n"
				"1:\t\n"

				"movw %1, %%ax\t\n"
				"movw %%ax, %%ds\t\n"
				"movw %%ax, %%es\t\n"
				"movw %%ax, %%fs\t\n"
				"movw %%ax, %%gs\t\n"
				"movw %%ax, %%ss\t\n"

				:
				: "g"(code), "g"(data)
    				: "eax"
			     );

}

/* Setup the descriptors */
void setup_descriptors()
{
	MM_REGS gdtr;

	gdt = (DESCRIPTOR *) alloc_physical_mem(1);
//	stss = (struct TSS *) alloc_physical_mem(1);

	if(!gdt)
		panic("setup_descriptors: Not Enough Memory to boot ManRiX\n");
	memset(gdt, 0, PAGE_SIZE);

	/* Setup GDT */
	setup_GDT();

 	gdtr.limit = (PAGE_SIZE - 1);
	gdtr.base = (linear_to_virtual(gdt));

	LGDT((MM_REGS *)(&gdtr));		/* Now, load the GDTR register */

	goto_segment(KERNEL_CODE, KERNEL_DATA);

	/*
	 * Update the FS and GS register.
	 * They always point to the User descriptors(user Address Space).
	 * It will be UseFul in Copying Data From Kernel to User and Vice
	 * Versa.
	 */
	write_fs(USER_DATA);
	write_gs(USER_DATA);


	memset(&stss, 0, sizeof(struct TSS) * MAX_CPU);
}

#ifdef MANRIX_SMP
/* Set the Gdt for Application Processors */
void ap_gdt_init()
{
	MM_REGS gdtr;

 	gdtr.limit = (PAGE_SIZE - 1);
	gdtr.base = (linear_to_virtual(gdt));

	LGDT((MM_REGS *)(&gdtr));		/* Now, load the GDTR register */

	goto_segment(KERNEL_CODE, KERNEL_DATA);

	/*
	 * Update the FS and GS register.
	 * They always point to the User descriptors(user Address Space).
	 * It will be UseFul in Copying Data From Kernel to User and Vice
	 * Versa.
	 */
	write_fs(USER_DATA);
	write_gs(USER_DATA);

}
#endif /* MANRIX_SMP */
