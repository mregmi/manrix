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
 * File:- multiboot.c
 * 		Just do some Multiboot Checks.
 */

#include <kernel/types.h>
#include <kernel/kernel_stdio.h>
#include <asm/multiboot.h>

/*
 * The Multiboot Information passed by the BootLoader.
 */
struct multiboot_info *minfo = NULL;

/*
 * Just do some Checks and get
 * the MultiBoot Information.
 */
void multiboot_parse(u32 magic, u32 info_add)
{
	char *cmdline;


	if(magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		panic("ManRiX is NOT loaded properly.\n	Please use multiboot complaint Boot loaders like GRUB.\n");
	}

	minfo = (struct multiboot_info *)info_add;
	cmdline = (char *)minfo->cmdline;

	kprintf("ManRiX Microkernel Loaded as '%s'.\n", cmdline);
}
