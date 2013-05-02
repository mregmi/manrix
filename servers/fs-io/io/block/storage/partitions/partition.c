/*
 * FS and I/O manager for XXX OS
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
 *
 */
/*
 * File: partition.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "blkdev.h"
#include "partition.h"

typedef int (* strategy_t)(struct operation *);
strategy_t devstrat = NULL;

/* Read the MBR */
int read_mbr(dev_t dev, int sectsize, MBR *mbr)
{
	int ret;

	ret = readsect(dev, 0, (void *)mbr, sectsize);
	if(ret < 0)
		return -1;

//	printf(" %x %x", mbr->signature, mbr->part[0].systemId);
	if(mbr->signature != 0xAA55)
	{
		printf(" Invalid Signature in MBR on disk %d %X \n", DISKNO(dev), mbr->signature);
		return -1;
	}

	return 0;
}

static inline int read_primary(MBR *mbr, struct partition_info *pinfo)
{
	PARTITION *part;
	int index, ebr = -1;

	for(index = 1; index <= 4; index++)
	{
		part = &mbr->part[index];
		pinfo->start = part->relativeSectors;
		pinfo->size = part->totalSectors;
		pinfo->ptype = part->systemId;
		pinfo->flag = part->bootIndicator;
	printf(" Part %x,  size %d MB lba %d\n", part->systemId, part->totalSectors/(2 * 1024), part->relativeSectors);
		if((pinfo->ptype == PARTITION_EXTENDED) ||
			(pinfo->ptype == PARTITION_EXTENDED1))
			ebr = index;
	}

	return ebr;
}

static inline int read_extended(dev_t dev, int sectsize, PARTITION *p, struct partition_info *pinfo)
{
	MBR ebr;
	PARTITION *p1, *p0;
	int ret, index = 5;
	uint32_t ebrBase, nextPart, ebr2 = 0;

	ebrBase = p->relativeSectors;

	ret = readsect(dev, ebrBase, &ebr, sectsize);
	if(ret < 0)
		return ret;

	do{
		p0 = &ebr.part[0];
		p1 = &ebr.part[1];

		pinfo[index].ptype = p0->systemId;
		pinfo[index].size = p0->totalSectors;
		pinfo[index].flag = p0->bootIndicator;
		pinfo[index].start = p0->relativeSectors + ebrBase + ebr2;
printf("Part %x,  size %d MB lba %d\n", p0->systemId, p0->totalSectors/(2 * 1024),p0->relativeSectors);
		ebr2 = p1->relativeSectors;
		nextPart = p1->relativeSectors + ebrBase;
		readsect(dev, nextPart, &ebr, sectsize);
		index++;
		if(index >= 255)
		{
			printf("Too many partitions. Cannot handle more than 256.\n");
			break;
		}
	}while (ebr.part[1].systemId != 0x00);

	pinfo[index].ptype = ebr.part[0].systemId;
	pinfo[index].size = ebr.part[0].totalSectors;
	pinfo[index].flag = p0->bootIndicator;
	pinfo[index].start = ebr.part[0].relativeSectors + ebrBase +ebr2;

	return (index + 1);
}


int read_partitions(dev_t dev, int sectsize, MBR *mbr, struct partition_info *pinfo)
{
	PARTITION *ebr;
	int index, ret = 0;

	index = read_primary(mbr, pinfo);
	if(index == -1)
		return 0;

	ebr = &mbr->part[index];
	ret = read_extended(dev, sectsize, ebr, pinfo);

	return ret;
}

struct partition_info *initialize_partitions(dev_t device, int sectorsize, int (* strategy)(struct operation *))
{
	MBR mbr;
	struct partition_info *pinfo;
	int ret;
	
	devstrat = strategy;

	ret = read_mbr(device, sectorsize, &mbr);
	if(ret < 0)
		return 0;

	pinfo = (struct partition_info *) malloc(sizeof(struct partition_info) * 256);
	memset(pinfo, 0, sizeof(struct partition_info) * 256);
		
	ret = read_partitions(device, sectorsize, &mbr, pinfo);
	if(ret < 0)
		return 0;

	
	pinfo = (struct partition_info *)realloc(pinfo, sizeof(struct partition_info) * ret);
	
	return pinfo;
}

int readsect(dev_t dev, unsigned long lba, void *data, int sectsize)
{
	struct operation op;
	int ret;

	op.device = dev;
	op.type = BLK_READ;
	op.block = lba;
	op.blksize = sectsize;
	op.buffer = data;

	ret = devstrat(&op);
	
	return ret;
}
