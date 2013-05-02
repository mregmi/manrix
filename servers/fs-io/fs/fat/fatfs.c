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
 * File: fatfs.c
 * 		FAT File System Implementation.
 */

#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#include "vfs.h"
#include "fatfs.h"

/*
 * Read the fat boot sector.
 */
int read_fat_boot(struct vfs *vfs)
{
	struct fat_super_block *bs;
	struct fat32bootsector *fgen;
	int ret, ok = 0;
	
	bs = &vfs->fatsb;
	
	ret = blk_read(vfs->vfs_dev, 1, BSSIZE, bs->bsptr);
	if(ret < 0)
	{
		DEBUG("Error reading boot sector\n");
		return ret;
	}
	fgen = (struct fat32bootsector *)bs->bsptr;
	
	/* Check if it is FAT file system */
	if((fgen->BS_jmpBoot[0] == 0xEB) && (fgen->BS_jmpBoot[2] == 0x90))
		ok = 1;
	else if(fgen->BS_jmpBoot[0] == 0xE9)
		ok = 1;
	
	if(!ok)
	{
		DEBUG("Invalid Jump instruction. Not a valid FAT filesystem \n");
		return	-1;	
	}
	bs->fat_type = get_fat_type(bs->bsptr);
	vfs->blk_size = fgen->BPB_BytsPerSec * fgen->BPB_SecPerClus;
	
	return 0;
}

/*
 * Determine the type of the FAT.
 */
int get_fat_type(struct fat32bootsector *fatbs)
{
	unsigned long nrootsects, fatsz, total_sects, data_sects, clusters;

	nrootsects = ((fatbs->BPB_RootEntCnt * 32) + (fatbs->BPB_BytsPerSec - 1)) / fatbs->BPB_BytsPerSec;
	if(fatbs->BPB_FATSz16 != 0)
		fatsz = fatbs->BPB_FATSz16;
	else
		fatsz = fatbs->BPB_FATSz32;

	if(fatbs->BPB_TotSec16 != 0)
		total_sects = fatbs->BPB_TotSec16;
	else
		total_sects = fatbs->BPB_TotSec32;

	data_sect = total_sects - (fatsz->BPB_RsvdSecCnt + (BPB_NumFATs * fatsz) + nrootsects);
	clusters = data_sect/fatbs->BPB_SecPerClus;

	if(clusters < 4085)
		return TYPE_FAT12;
	else if(clusters < 65525)
		return TYPE_FAT16;
	else
		return TYPE_FAT32;
}
