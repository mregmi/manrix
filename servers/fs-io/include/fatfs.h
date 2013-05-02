/*
 * FS and I/O manager for XXX OS
 * Copyright (C) 2004, 2005	Manish Regmi (regmi dot manish at gmail dot com)
 *				Rajesh R.C (rajee5283 at hotmail dot com)
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
 *
 *
 * File: fatfs.h
 * 		Declaration for FAT File System.
 */
#ifndef __FATFS_H
#define __FATFS_H

#include <stdint.h>

#define TYPE_FAT12		1
#define TYPE_FAT16		2
#define TYPE_FAT32		3

#define BSSIZE			512

struct fat32bootsector{
	uint8_t		BS_jmpBoot[3];	/* Jump instruction */
	uint8_t		BS_OEMName[8];	/* MSWIN4.1 */

	/* BPB */
	uint16_t	BPB_BytsPerSec;	/* Count of bytes per sector. */
	uint8_t		BPB_SecPerClus;	/* Number of sectors per allocation unit */
	uint16_t	BPB_RsvdSecCnt;	/* Number of reserved sectors in the Reserved region */
	uint8_t		BPB_NumFATs;	/* The count of FAT data structures on the volume. */
	uint16_t	BPB_RootEntCnt;	/* For FAT32 volumes,this field must be set to 0. */
	uint16_t	BPB_TotSec16;	/* For FAT32 volumes, this field must be 0. */
	uint8_t		BPB_Media;		/* 0xF8 is the standard value for fixed (non-removable) media.
									For removable media, 0xF0 is frequently used.*/
	uint16_t	BPB_FATSz16;	/* On FAT32 volumes this field must be 0 */
	uint16_t	BPB_SecPerTrk;	/* Sectors per track */
	uint16_t	BPB_NumHeads;	/* Number of heads */
	uint32_t	BPB_HiddSec;	/* Count of hidden sectors preceding the partition that
									contains this FAT volume. */
	uint32_t	BPB_TotSec32;	/* This field is the new 32-bit total count of sectors on the volume. */
	uint32_t	BPB_FATSz32;	/* This field is the 32-bit count of sectors occupied by ONE FAT.*/
	uint16_t	BPB_ExtFlags;	/* */
	uint16_t	BPB_FSVer;		/* High byte is major revision number.
									Low byte is minor revision number. */
	uint32_t	BPB_RootClus;	/* the cluster number of the first cluster of the root directory. */
	uint16_t	BPB_FSInfo;		/* Sector number of FSINFO structure in the
									reserved area of the FAT32 volume. */
	uint16_t	BPB_BkBootSec;	/* sector number of a copy of the boot record. */
	uint8_t		BPB_Reserved[12];

	/* Extended BPB */
	uint8_t		BS_DrvNum;		/* Int 0x13 drive number (e.g. 0x80). */
	uint8_t		BS_Reserved1;	/* Reserved (used by Windows NT). */
	uint8_t		BS_BootSig;		/* Extended boot signature (0x29). */
	uint32_t	BS_VolID;		/* Volume serial number. */
	uint8_t		BS_VolLab[11];	/* Volume label. */
	uint8_t		BS_FilSysType[8];	/* One of the strings FAT12 ,FAT16 , or FAT. */


	uint8_t		bCode[420];
	uint16_t	signature;
};

struct fat16bootsector{
	uint8_t		BS_jmpBoot[3];	/* Jump instruction */
	uint8_t		BS_OEMName[8];	/* MSWIN4.1 */

	/* BPB */
	uint16_t	BPB_BytsPerSec;	/* Count of bytes per sector. */
	uint8_t		BPB_SecPerClus;	/* Number of sectors per allocation unit */
	uint16_t	BPB_RsvdSecCnt;	/* Number of reserved sectors in the Reserved region */
	uint8_t		BPB_NumFATs;	/* The count of FAT data structures on the volume. */
	uint16_t	BPB_RootEntCnt;	/* For FAT32 volumes,this field must be set to 0. */
	uint16_t	BPB_TotSec16;	/* For FAT32 volumes, this field must be 0. */
	uint8_t		BPB_Media;	/* 0xF8 is the standard value for fixed (non-removable)
					   media. For removable media, 0xF0 is frequently used.*/
	uint16_t	BPB_FATSz16;	/* On FAT32 volumes this field must be 0 */
	uint16_t	BPB_SecPerTrk;	/* Sectors per track */
	uint16_t	BPB_NumHeads;	/* Number of heads */
	uint32_t	BPB_HiddSec;	/* Count of hidden sectors preceding the partition
					   that	contains this FAT volume. */
	uint32_t	BPB_TotSec32;	/* This field is the new 32-bit total count of sectors on the volume. */

	/* Extended BPB */
	uint8_t		BS_DrvNum;		/* Int 0x13 drive number (e.g. 0x80). */
	uint8_t		BS_Reserved1;	/* Reserved (used by Windows NT). */
	uint8_t		BS_BootSig;		/* Extended boot signature (0x29). */
	uint32_t	BS_VolID;		/* Volume serial number. */
	uint8_t		BS_VolLab[11];	/* Volume label. */
	uint8_t		BS_FilSysType[8];	/* One of the strings FAT12 ,FAT16 , or FAT. */

	uint8_t		bCode[448];
	uint16_t		signature;

};


struct fat_super_block {
	int	ino_val;		/* inode value allocation counter */
	int fat_type;
	char fatbs[BSSIZE];
	void *bsptr;			/* Pointer to above data. Just for simplicity */
};

#endif	/* __FATFS_H */
