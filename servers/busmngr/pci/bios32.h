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
 * File: bios32.h
 * 		32 bit Bios Directory Service declarations
 */

#ifndef __BIOS32_H
#define __BIOS32_H

#include <stdint.h>

/* BIOS32 Signature: "_32_" */
#define BIOS32_SIG		(('_' << 0) + ('3' << 8) + ('2' << 16) + ('_' << 24))

/*
 * ROM address where the BIOS32 Directory Structure is searched.
 */
#define ROM_START		0xE0000
#define ROM_END			0xFFFFF

/* 16 Bytes = 1 paragraph */
#define PARAGRAPH_SIZE	16

/* Structure for Bios32 Servie Directory */
struct bios32{
	uint32_t signature;	/* BIOS32 Signature, ASCII "_32_" */
	uint32_t entry;		/* BIOS32 service Entry Point, 32bit Linear address */
	uint8_t  revision;	/* Revision Level; Currently 0h. */
	uint8_t  length;	/* Length of the Header in Paragraph */
	uint8_t  checksum;	/* Checksum; all the bytes in Header must add to zero */
	uint8_t  reserved[5];	/* Reserved to 0 */
};

/* The service values returned by BIOS32 */
struct bios32_service{
	uint32_t base;		/* Base address of Bios Service */
	uint32_t length;	/* Length of 32Bit Bios Service */
	uint32_t offset;	/* Offset (from base) of the Entry Point */
	uint32_t address;	/* Actual address */
};

/* For lcall */
struct farcall{
	unsigned long address;	/* offset */
	unsigned short desc;	/* segment */
};

extern struct farcall call;

extern unsigned long rom_start, rom_end;

extern struct bios32 *check_bios32();
extern unsigned long map_rom(unsigned long, unsigned long );
extern int BIOS32_exist(uint32_t , struct bios32_service *, struct bios32 *);

#endif /* __BIOS32_H */
