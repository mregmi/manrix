/**
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
/**
 * File: pci.c
 * 		Peripheral Component Interconnect (PCI) driver.
 *	TODO: This file Should go to architecture dependent part (???)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bios32.h"
#include "pci.h"


/**
 * This function allows the caller to determine whether the PCI BIOS
 * interface function set is present and what the current interface
 * version level is. It also provides information about what hardware
 * mechanism for accessing configuration space is supported, and
 * whether or not the hardware supports generation if PCI Special Cycles.
 */
int PCIBIOS_ispresent(struct bios32_service *pcisrv)
{
	unsigned long retval;	/* eax */
	unsigned long version;	/* ebx */
	unsigned long signature;	/* edx */
	int ret;

	call.address = pcisrv->address;

	asm volatile(
		"lcall *(%%edi) \n\t"
		"jc 1f \n\t"
		"xor %%ah, %%ah\n\t"
		"1:"
		:"=a"(retval), "=b"(version), "=d"(signature)
		: "0"(PCI_BIOS_PRESENT), "D"(&call)
		);

	if(signature != PCI_SIGNATURE)
	{
		printf("Signature Not Valid\n");
		return 1;
	}

	ret = (retval >> 8) & 0xFF;	/* Only AH contains return code */
	if(ret != SUCCESSFUL)
	{
		printf("Error in Detecting PCIBIOS\n");
	}

	printf("PCIBIOS Version %d.%d Detected.\n", (version >> 8) & 0xFF, version & 0xFF);

	return 0;
}

/**
 * This function returns the location of PCI devices that have
 * a specific Device ID ans Vendor ID. Given a Vendor ID, Device ID
 * and an Index (N), the function returns the Bus Number, Device Number,
 * and Function Number of the Nth Device/Function whose Vendor ID and
 * Device ID match the input parameters.
 */
int find_PCI_device(struct bios32_service *pcisrv,
		uint16_t device, uint16_t vendor, uint16_t index,
		uint8_t *bus, uint8_t *dev)
{
	unsigned long retval;	/* eax */
	unsigned long out;	/* ebx */
	int ret;

	call.address = pcisrv->address;

	asm volatile(
		"lcall *(%%edi) \n\t"
		"jc 1f \n\t"
		"xor %%ah, %%ah\n\t"
		"1:"
		:"=a"(retval), "=b"(out)
		: "0"(FIND_PCI_DEVICE), "c"(device), "d"(vendor), "D"(&call), "S"(index)
		);

	ret = (retval >> 8) & 0xFF;	/* Only AH contains return code */

	*bus = (out >> 8) & 0xFF;
	*dev = out & 0xFF;

	return ret;
}

/**
 * This function returns the location of PCI devices that have a
 * specific Class Code. Given a Class Code and a Index (N), the function
 * returns the Bus number, Device Number, and Function Number of the Nth
 * Device/Function whose Class Code matches the input parameters.
 */
int find_PCI_class_code(struct bios32_service *pcisrv,
		uint32_t classcode, uint16_t index,
		uint8_t *bus, uint8_t *dev)
{
	unsigned long retval;	/* eax */
	unsigned long out;	/* ebx */
	int ret;

	call.address = pcisrv->address;

	asm volatile(
		"lcall *(%%edi) \n\t"
		"jc 1f \n\t"
		"xor %%ah, %%ah\n\t"
		"1:"
		:"=a"(retval), "=b"(out)
		: "0"(FIND_PCI_CLASS_CODE), "D"(&call), "c"(classcode), "S"(index)
		);

	ret = (retval >> 8) & 0xFF;	/* Only AH contains return code */

	*bus = (out >> 8) & 0xFF;
	*dev = out & 0xFF;

	return ret;
}

/**
 * This function allows for generation of PCI special cycles.
 * The generated special cycle will be broadcast on a specific
 * PCI bus in the system.
 */
int generate_special_cycle(struct bios32_service *pcisrv,
		uint8_t bus, uint32_t data)
{
	unsigned long retval;	/* eax */

	call.address = pcisrv->address;

	asm volatile(
		"lcall *(%%edi) \n\t"
		"jc 1f \n\t"
		"xor %%ah, %%ah\n\t"
		"1:"
		:"=a"(retval)
		: "0"(GENERATE_SPECIAL_CYCLE), "D"(&call), "b"(bus), "d"(data)
		);

	return ((retval >> 8) & 0xFF);
}

/**
 * This function allows reading individual bytes from the configuration
 * space of a specific device.
 */
int read_configuration_byte(struct bios32_service *pcisrv,
		uint8_t bus, uint8_t device, uint8_t reg, uint8_t *data)
{
	unsigned long retval;	/* eax */
	unsigned long out;	/* ecx */
	int ret;
	uint32_t busdev;

	call.address = pcisrv->address;
	busdev = (bus << 8) | device;

	asm volatile(
		"lcall *(%%esi) \n\t"
		"jc 1f \n\t"
		"xor %%ah, %%ah\n\t"
		"1:"
		:"=a"(retval), "=c"(out)
		: "0"(READ_CONFIG_BYTE), "b"(busdev), "D"((long)reg), "S"(&call)
		);

	ret = (retval >> 8) & 0xFF;	/* Only AH contains return code */

	*data = out & 0xFF;

	return ret;
}

/**
 * This function allows reading individual words from the configuration
 * space of a specific device. The Register Number parameter must be a
 * multiple of two (i.e., bit 0 must be set to 0).
 */
int read_configuration_word(struct bios32_service *pcisrv,
		uint8_t bus, uint8_t device, uint8_t reg, uint16_t *data)
{
	unsigned long retval;	/* eax */
	unsigned long out;	/* ecx */
	int ret;
	uint32_t busdev;

	busdev = (bus << 8) | device;
	call.address = pcisrv->address;

	asm volatile(
		"lcall *(%%esi) \n\t"
		"jc 1f \n\t"
		"xor %%ah, %%ah\n\t"
		"1:"
		:"=a"(retval), "=c"(out)
		: "0"(READ_CONFIG_WORD), "b"(busdev), "D"((uint32_t)reg), "S"(&call)
		);

	ret = (retval >> 8) & 0xFF;	/* Only AH contains return code */

	*data = out & 0xFFFF;

	return ret;
}

/**
 * This function allows reading individual bytes from the configuration
 * space of a specific device. The Register Number parameter must be a
 * multiple of four (i.e., bit 0 and 1 must be set to 0).
 */
int read_configuration_dword(struct bios32_service *pcisrv,
		uint8_t bus, uint8_t device, uint8_t reg, uint32_t *data)
{
	unsigned long retval;	/* eax */
	unsigned long out;	/* ecx */
	int ret;
	uint32_t busdev;

	busdev = (bus << 8) | device;
	call.address = pcisrv->address;

	asm volatile(
		"lcall *(%%esi) \n\t"
		"jc 1f \n\t"
		"xor %%ah, %%ah\n\t"
		"1:"
		:"=a"(retval), "=c"(out)
		: "0"(READ_CONFIG_DWORD), "b"(busdev), "D"((uint32_t)reg), "S"(&call)
		);

	ret = (retval >> 8) & 0xFF;	/* Only AH contains return code */

	*data = out;

	return ret;
}


/**
 * This function allows writing individual bytes from the
 * configuration space of a specific device.
 */
int write_configuration_byte(struct bios32_service *pcisrv,
		uint8_t bus, uint8_t device, uint8_t reg, uint8_t data)
{
	unsigned long retval;	/* eax */
	int ret;
	uint32_t busdev;

	busdev = (bus << 8) | device;
	call.address = pcisrv->address;

	asm volatile(
		"lcall *(%%esi) \n\t"
		"jc 1f \n\t"
		"xor %%ah, %%ah\n\t"
		"1:"
		:"=a"(retval)
		: "0"(WRITE_CONFIG_BYTE), "b"(busdev), "D"((uint32_t)reg), "c"((uint32_t)data), "S"(&call)
		);

	ret = (retval >> 8) & 0xFF;	/* Only AH contains return code */

	return ret;
}

/**
 * This function allows writing individual words from the
 * configuration space of a specific device. The Register Number
 * parameter must be a multiple of two (i.e., bit 0 must be set to 0).
 */
int write_configuration_word(struct bios32_service *pcisrv,
		uint8_t bus, uint8_t device, uint8_t reg, uint16_t data)
{
	unsigned long retval;	/* eax */
	int ret;
	uint32_t busdev;

	busdev = (bus << 8) | device;
	call.address = pcisrv->address;

	asm volatile(
		"lcall *(%%esi) \n\t"
		"jc 1f \n\t"
		"xor %%ah, %%ah\n\t"
		"1:"
		:"=a"(retval)
		: "0"(WRITE_CONFIG_WORD), "S"(&call), "b"(busdev), "D"((uint32_t)reg), "c"(data)
		);

	ret = (retval >> 8) & 0xFF;	/* Only AH contains return code */

	return ret;
}

/**
 * This function allows writing individual dwords from the configuration
 * space of a specific device. The Register Number parameter must be a
 * multiple of four (i.e., bit 0 and 1 must be set to 0).
 */
int write_configuration_dword(struct bios32_service *pcisrv,
		uint8_t bus, uint8_t device, uint8_t reg, uint32_t data)
{
	unsigned long retval;	/* eax */
	int ret;
	uint32_t busdev;

	busdev = (bus << 8) | device;
	call.address = pcisrv->address;

	asm volatile(
		"lcall *(%%esi) \n\t"
		"jc 1f \n\t"
		"xor %%ah, %%ah\n\t"
		"1:"
		:"=a"(retval)
		: "0"(WRITE_CONFIG_DWORD), "S"(&call), "b"(busdev), "D"((uint32_t)reg), "c"(data)
		);

	ret = (retval >> 8) & 0xFF;	/* Only AH contains return code */

	return ret;
}
