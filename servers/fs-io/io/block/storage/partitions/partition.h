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
 *
 * File: partition.h
 */

#ifndef __FSIO_PARTITION_H
#define __FSIO_PARTITION_H

#include "blkdev.h"
#include <stdint.h>

#define PARTITION_EXTENDED		0x05
#define PARTITION_EXTENDED1		0x0F


/* Partition structure */
struct tagPARTITION {
	uint8_t  bootIndicator;
	uint8_t  startingHead;
	uint16_t start;
	uint8_t  systemId;
	uint8_t  endingHead;
	uint16_t end;
	uint32_t relativeSectors;
	uint32_t totalSectors;
} __attribute__((packed));

typedef struct tagPARTITION PARTITION;

/* Stores the MBR and EBR information */
struct tagMBR {
	char res[446];
	PARTITION part[4];
	uint16_t signature;
} __attribute__((packed));

typedef struct tagMBR MBR;

extern int readsect(dev_t , unsigned long , void *, int);

extern struct partition_info *initialize_partitions(dev_t, int, int (* strategy)(struct operation *));


#endif  /* __FSIO_PARTITION_H */
