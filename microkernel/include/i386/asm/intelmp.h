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
 * File:- intelmp.h
 * 	The Intel Multiprocessor spec related declarations.
 **/


#ifndef __ASM_INTELMP_H
#define __ASM_INTELMP_H

#ifndef __MANRIX_TYPES_H
 #include <kernel/types.h>
#endif

/* Signature for MP floating pointer "_MP_" */
#define MP_SIGNATURE	(('_') + ('P' << 8) + ('M' << 16) + ('_' << 24))

/* Signature for MP configuration "PMCP" */
#define MP_CONFIG_SIGNATURE	(('P') + ('C' << 8) + ('M' << 16) + ('P' << 24))

/* The size of different entries */
#define MP_ENTRY_SIZE_PROCESSOR		20
#define MP_ENTRY_SIZE_OTHER			8

/* Entry types in the configuration */
#define MP_ENTRY_PROCESSOR			0
#define MP_ENTRY_BUS				1
#define MP_ENTRY_IOAPIC				2
#define MP_ENTRY_IO_INT_ASSIGN		3
#define MP_ENTRY_LOCAL_INT_ASSIGN	4

/* MP floating pointer structure */
struct mp_floating {
	u32		signature;
	u32		physaddr;
	u8 		length, spec_rev,
			checksum, mp_feature1;
	u32		mp_feature;
};

/* MP configuration Header */
struct mp_config_header {
	u32		signature;
	u16		base_len;
	u8		spec_rev, checksum;
	char	oem_id[8];
	char	product_id[12];
	u32		oem_ptr;
	u16		oem_size;
	u16		entry_count;
	u32		apic_ptr;
	u16		extended_len;
	u8		extended_checksum;
	u8		reserved;
};

/* Processor entry structure */
struct entry_processor {
	u8 type, lapic_id, lapic_ver, cpu_flags;
	u32	signature;
	u32	features;
	u32	reserved0, reserved1;
};

/* Bus entry structure */
struct entry_bus {
	u8 type, bus_id;
	char string[6];
};

/* I/O APIC entry Structure */
struct entry_ioapic {
	u8 type, ioapic_id, ioapic_ver, ioapic_flags;
	u32 ioapic_ptr;
};

/* Generic processor structure for a multiprocessor */
struct mp_processor{
	u32 signature;
	u32 features;
	u8 lapic_id, cpu_flags;
};

#endif	/* __ASM_INTELMP_H */
