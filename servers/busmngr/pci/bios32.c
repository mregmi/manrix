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
 * File: bios32.c
 * 		32 bit Bios Directory Service.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "bios32.h"

/*
 * The Starting and Ending address of ROM after mapping.
 * After mapping for the First time, it will be available
 * to the Process forever(i.e it wont be unmapped).
 */
unsigned long rom_start, rom_end;

/* Segment Offset for User Program */
struct farcall call = {0, 0x1b};

/**
 * Verify the checksum.
 * the sum of all the bytes must add to zero
 **/
static inline int verify_checksum(char *data, int len)
{
	int index;
	unsigned char sum = 0;

	for(index = 0; index < len; index++)
	{
		sum += data[index];
	}

	return sum;
}

/*
 * First check for the existance of BIOS32 service.
 * The Caller Must make sure the ROM address is addressable
 * by the running thread.
 */
struct bios32 *check_bios32()
{
	struct bios32 *bios;
	char *entry;
	unsigned long address;
	int index, length;
	long sum = 0;

	for(address = rom_start; address <= rom_end; address += PARAGRAPH_SIZE)
	{
		bios = (struct bios32 *)address;

		/* Check the Signature */
		if(bios->signature != BIOS32_SIG)
			continue;

		/*
		 * Now we Found the Signature but we cannot rely on it.
		 * Use the checksum to verify.
		 */
		if(bios->length != 1)
		{
			printf(" Length is Not 1 ");
			continue;
		}
		else if(bios->revision != 0)
		{
			printf(" Revision is Not 0 ");
			continue;
		}

		length = bios->length * PARAGRAPH_SIZE;
		
		sum = verify_checksum((char *)bios, length);
		if(sum != 0)
		{
			printf("Checksum is Invalid (%d)\n", sum);
//			continue;
		}

		return bios;
	}

	return NULL;
}

/*
 * Map the ROM of the PC so they are addressable.
 */
unsigned long map_rom(unsigned long start, unsigned long size)
{
	unsigned long mapped;

	/* Mapping ROM area */
	mapped = (unsigned long)mmap((void *)start, size, PROT_READ,
				MAP_ANON|MAP_SHARED, -1, 0);

	return mapped;
}

/*
 * BIOS32 Call, the Component Existence Function.
 * Checks if the Specfic BIOS32 Service exists or not.
 */
int BIOS32_exist(uint32_t id, struct bios32_service *service, struct bios32 *header)
{
	int retval;

	call.address = header->entry;

	asm volatile(
		"lcall *(%%edi) \n\t"
		:"=a"(retval), "=b"(service->base), "=c"(service->length), "=d"(service->offset)
		: "0"(id), "1"(0), "D"(&call)
		);

	retval &= 0xFF;	/* Only AL contains return code */

	return retval;
}
