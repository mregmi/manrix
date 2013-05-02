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
 * File:- multiboot.h
 * 	ManRiX is a multiboot complaint OS and can be loaded by a
 * multiboot complaint boot loader. We are using Grub as a boot
 * loader which is probably the best boot loader that exists today.
 * This file contains declarations as defined in multiboot
 * specification of GRUB. You can get it on
 * http://www.gnu.org/software/grub/grub.html
 */

#ifndef __MANRIX_MULTIBOOT_H
#define __MANRIX_MULTIBOOT_H

/* The magic number for the Multiboot header. */
#define MULTIBOOT_HEADER_MAGIC			0x1BADB002

/* The magic number passed by a Multiboot-compliant boot loader. */
#define MULTIBOOT_BOOTLOADER_MAGIC      	0x2BADB002

/* Multiboot Header Flags */
#define MULTIBOOT_HEADER_FLAGS			0x0003

/* Multiboot Header Checksum */
#define MULTIBOOT_HEADER_CHECKSUM		(0 - (MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS))


#ifndef __ASSEMBLY__

#ifndef __MANRIX_TYPES_H
#include <kernel/types.h>
#endif

/* multiboot header */
struct multiboot_header {
	u32		magic;
	u32		flags;
	u32		checksum;
	u32		header_addr;
	u32		load_addr;
	u32		load_end_addr;
	u32		bss_end_addr;
	u32		entry_addr;
};

/* section header table for ELF format kernel */
struct elf_section_header_table {
	u32		num;
	u32		strsize;
	u32		addr;
	u32		reserved;
};

/* Multiboot information */
struct multiboot_info {
	u32		flags;
	u32		mem_lower;
	u32		mem_upper;
	u32		boot_device;
	u32		cmdline;
	u32		mods_count;
	u32		mods_addr;
	struct elf_section_header_table	 elf_table;
	u32		mmap_length;
	u32		mmap_addr;
};

/* memory map information */
struct memory_map {
	u32		size;
	u32		base_addr_low;
	u32		base_addr_high;
	u32		length_low;
	u32		length_high;
	u32		type;
};

/* The module structure. */
struct module
{
  u32 mod_start;
  u32 mod_end;
  u32 string;
  u32 reserved;
};

typedef struct module *module_t;

extern void modules_init();
extern void arch_setup();

#endif	/* __ASSEMBLY__ */
#endif	/* __MANIX_MULTIBOOT_H */
