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
 * File:- smp.c
 * 	We currently support Intel Multiprocessors only.
 *	See Official Intel Documents for more information.
 **/
#include <kernel/config.h>
#include <string.h>
#include <kernel/vm.h>
#include <kernel/types.h>
#include <asm/intelmp.h>
#include <asm/x86.h>

struct mp_floating mp;
struct mp_processor processors[MAX_CPU] = {{0, 0, 0, 0}};

/* This File is needed only on SMP Systems */
#ifdef MANRIX_SMP

extern struct pmap kernel_map;
extern int N_PROCS;

/**
 * Verify the checksum.
 * the sum of all the bytes must add to zero
 **/
static inline int verify_checksum(char *data, int len)
{
	int index;
	u8 sum = 0;

	for(index = 0; index < len; index++)
	{
		sum += data[index];
	}

	return sum;
}

/**
 * Detect if the system is Intel Multiprocessor or not.
 * It searches for the MP floating pointer.
 **/
int detect_intel_mp()
{
	struct mp_floating *fl;
	int sum;

	/* First check in the Bios ROM */
	pmap_enter_range(&kernel_map, BIOS_ROM_START, BIOS_ROM_START+KERNEL_BASE, 0x20000, 0);
	for(fl = (struct mp_floating *)BIOS_ROM_START; (u32)fl < BIOS_ROM_END; fl++)
	{
		if(fl->signature != MP_SIGNATURE)
			continue;
		kprintf("%X ",fl->signature);
		memcpy((void *)&mp, fl, sizeof(struct mp_floating));
		sum = verify_checksum((char *)fl, sizeof(struct mp_floating));
		if(sum != 0)
		{
			kprintf("Invalid Checksum for Intel MP floating pointer structure %d \n", sum);
			continue;
		}

		pmap_remove(&kernel_map, BIOS_ROM_START+KERNEL_BASE, BIOS_ROM_END+KERNEL_BASE);
		return 0;
	}
	pmap_remove(&kernel_map, BIOS_ROM_START+KERNEL_BASE, BIOS_ROM_END+KERNEL_BASE);

	return -1;
}

int smp_init()
{
	struct mp_config_header *hdr;
	struct entry_processor *proc;
	int sum, nentries;
	u8 *entry;

	if(detect_intel_mp() < 0)
	{
		kprintf("Your System is not a Intel MP compatible. Falling back to UP mode.\n");
		return 0;
	}

	/* Ok, It is SMP System. Check for the total no of Processors */
	if(!mp.physaddr)
	{
		/* It is a Dual Processor System and use the default configuration */
		N_PROCS = 2;
		processors[0].lapic_id = 0;
		processors[0].cpu_flags = 0x02;	/* BSP */
		processors[1].lapic_id = 1;
		processors[1].cpu_flags = 0;	/* AP */

		return 0;
	}

	pmap_enter_range(&kernel_map, mp.physaddr, mp.physaddr+KERNEL_BASE, 0x1000, 0);

	hdr = (struct mp_config_header *)mp.physaddr;
	if(hdr->signature != MP_CONFIG_SIGNATURE)
	{
		kprintf("Invalid Signature in configuration table header\n");
		kprintf("Falling Back to UP mode.\n");
		return 0;
	}

	sum = verify_checksum((char *)hdr, sizeof(struct mp_config_header));
	if(sum)
	{
		kprintf("Invalid Checksum in configuration table header\n");
		kprintf("Falling Back to UP mode.\n");
		return 0;
	}

	entry = (u8 *)hdr + 1;
	N_PROCS = 0;
	for(nentries = 0; nentries < hdr->entry_count; nentries++)
	{
		switch(entry[0])
		{
			case 0:
				proc = (struct entry_processor *) entry;
				processors[N_PROCS].signature = proc->signature;
				processors[N_PROCS].features = proc->features;
				processors[N_PROCS].lapic_id = proc->lapic_id;
				processors[N_PROCS].cpu_flags = proc->cpu_flags;
				N_PROCS++;
				if(N_PROCS > MAX_CPU)
					break;

				entry += sizeof(struct entry_processor);
				break;
			default:
				break;
		}

	}

	pmap_remove(&kernel_map, mp.physaddr + KERNEL_BASE, mp.physaddr + KERNEL_BASE + 1000);

	return 0;
}

#endif /* MANRIX_SMP */
