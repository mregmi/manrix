/*
 * FS and I/O manager for ManRiX OS
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
 * File: ata.c
 *
 * The ATA disk driver
 */

#include "svrconfig.h"

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <stdint.h>
#include <stdlib.h>

#include <mrx/blkdev.h>
#include <asm/syscall.h>

#include "ata.h"
#include "blkdev.h"
#include "../partitions/partition.h"

#ifndef SEPERATE_IO_PROCESS
struct block_operations ideblkop =	{
			.d_open = ata_open,
			.d_close = ata_close,
			.d_strategy = ata_strategy,
			.d_size = ata_size
		};

#endif /* SEPERATE_IO_PROCESS */

struct diskinfo *device = NULL;

struct operation *prim = NULL;
struct operation *sec = NULL;

/* Which command to use for sector read and write. */
static int readcmd, writecmd;

/*
 * Check if the BSY bit is clear and wait until it is clear.
 * This checks if the drive is busy.
 */
static inline int check_busy(unsigned char disk)
{
	unsigned char data;
	int timeout = 100000;

	do{
		data = READ_REG(disk, ATA_STAT_COM_ADDR);
		if(!--timeout)
			return -1;

	}while(data & ATA_STAT_BSY);

	return 0;
}

/*
 * Check if the DRDY bit is set and wait until it is set.
 * This checks if the drive is ready to accept commands.
 */
static inline int check_drdy(unsigned char disk)
{
	unsigned char data;
	int timeout = 100000;

	do{
		data = READ_REG(disk, ATA_STAT_COM_ADDR);
		if(!--timeout)
			return -1;

	}while((data & ATA_STAT_DRDY) != ATA_STAT_DRDY);

	return 0;
}

/*
 * Check if the DRQ bit is set and wait until it is set.
 * This checks if the drive is ready to transfer a word
 * or byte of data between the host and the data port.
 */
static inline int check_drq(unsigned char disk)
{
	unsigned char data;
	int timeout = 100000;

	do{
		data = READ_REG(disk, ATA_STAT_COM_ADDR);
		if(!--timeout || (data & ATA_STAT_ERR))
			return -1;

	}while((data & ATA_STAT_DRQ) != ATA_STAT_DRQ);

	return 0;
}

void ata_soft_reset(unsigned char disk)
{
	/* Doing a software Reset */
	WRITE_DEVCTRL(ATA_DEVCTRL_DEF|ATA_DEVCTRL_SRST);

	check_busy(disk);
	/* Enable nIENT */
	WRITE_DEVCTRL(ATA_DEVCTRL_DEF|ATA_DEVCTRL_nIEN);
	check_busy(disk);
}
static inline void write_lba(unsigned char disk, unsigned long lba)
{
	unsigned char data;

	data = (unsigned char)(lba & 0xFF);
	WRITE_REG(disk, ATA_SECT_NUM_ADDR, data);

	data = (unsigned char)((lba >> 8) & 0xFF);
	WRITE_REG(disk, ATA_CYL_LOW_ADDR, data);

	data = (unsigned char)((lba >> 16) & 0xFF);
	WRITE_REG(disk, ATA_CYL_HIGH_ADDR, data);

	data = (unsigned char)((lba >> 24) & 0xF);
	data |= ATA_DEV_DEF | ATA_DEV_LBA | ((disk & 1) ? ATA_DEV_DEV : 0);
	WRITE_REG(disk, ATA_DEV_HEAD_ADDR, data);

}


/* Wait for the request to complete and transfer data */
static inline int waitnread_data(struct operation *ops, int sector_size)
{
	int reg;
	int length, disk = DISKNO(ops->device);
//	uint16_t *dt = ops->buffer;
//	printf("caller is %d", ops->caller);
//	syscall_sleep_unint(ops->caller);

	if(READ_REG(disk, ATA_STAT_COM_ADDR) & ATA_STAT_ERR)
	{
		reg = READ_REG(disk, ATA_ERR_REG_ADDR);
		printf("ATA read error %d ",reg);
		return -reg;
	}

	/* Now read the data */
	length = (ops->blksize * sector_size);
//	printf(" readingd %d data from disk %d lba %x. ",length, ops->disk, ops->lba);

	check_drq(disk);
	READ_DATA(disk, ATA_DATA_REG_ADDR, ops->buffer, length);
//	printf("\n %x %X str %s ", dt[0], dt[255], &dt[290]);
	return 0;
}


void ata_intr_primary()
{
//	printf("Interrupt service ");
	if(!prim)
		goto intr_end;

//	syscall_wake_unint(prim->caller);

intr_end:
	finish_intr();
}

void ata_intr_secondary()
{
//	printf("Interrupt service2 for ");
	if(!sec)
		goto intr2_end;

//	syscall_wake_unint(sec->caller);

intr2_end:
	finish_intr();
}
/*
void *ata_thr(void *v)
{
	syscall_change_iopl(3);
	syscall_IRQ_request(IRQ_PRIMARY_IDE, ata_intr_primary);
	syscall_IRQ_request(IRQ_SECONDARY_IDE, ata_intr_secondary);

	IRQ_init(IRQ_PRIMARY_IDE);
	IRQ_init(IRQ_SECONDARY_IDE);

//	printf("ata done ");
	do{

		syscall_IRQ_wait();

	}while(1 == 1);

	return NULL;
}
*/

/*
 * Check if the multi read/write commands are supported.
 */
int check_cmd_supported(int disk)
{
	WRITE_REG(disk, ATA_SECT_COUNT_ADDR, 4);

	write_lba(disk, 0);

	check_drdy(disk);
	WRITE_REG(disk, ATA_STAT_COM_ADDR, ATA_CMD_READ_MULTI);
	if(READ_REG(disk, ATA_STAT_COM_ADDR) & ATA_STAT_ERR)
	{
		readcmd = ATA_CMD_READ_SECT;
		writecmd = ATA_CMD_WRITE_SECT;
		printf("Multi Read/Write Not supported\n");
	}
	else
	{
		printf("Multi Read/Write supported\n");
		readcmd = ATA_CMD_READ_MULTI;
		writecmd = ATA_CMD_WRITE_MULTI;
	}
}

/* Initialize ata driver */
int ata_init()
{
//	int ret;
	IRQ_request(IRQ_PRIMARY_IDE, ata_intr_primary);
	IRQ_request(IRQ_SECONDARY_IDE, ata_intr_secondary);
//	printf(" done ");
	device = (struct diskinfo *) malloc(sizeof(struct diskinfo) * 4);
	memset(device, 0, sizeof(struct diskinfo) * 4);
	if(!device)
	{
		printf("ATA DRIVER: Not enough Memoy");
		exit(ENOMEM);
	}
	prim = NULL;
	sec = NULL;
	check_cmd_supported(0);
	disk_detect();
//	printf("DONE nvg\n");
#ifdef SEPERATE_IO_PROCESS
	request_ionamespace();
#else
	register_blockdev(1, &ideblkop, 4);
#endif
	return 0;
}

/* exit ata driver */
int ata_exit()
{
	free(device);
#ifndef SEPERATE_IO_PROCESS
	unregister_blockdev(1);
#endif
	return 0;
}

/* Open an IDE device */
#ifndef SEPERATE_IO_PROCESS
int ata_open(struct operation *op)
{
	return 0;
}

/* close an IDE device */
int ata_close(struct operation *op)
{
	return 0;
}

int ata_size(struct operation *op)
{
	//return disk size;
	return 0;
}
#endif


/* Identify the Drive Parameters */
static inline int read_driveid(int disk, uint16_t *buf)
{
	int index;
//	unsigned char data, err;

	if(!buf)
		return -1;

	while(READ_REG(disk, ATA_STAT_COM_ADDR) & ATA_STAT_BSY)
		;

	WRITE_REG(disk, ATA_DEV_HEAD_ADDR, ((disk & 1) ? (ATA_DEV_DEF|ATA_DEV_DEV) : ATA_DEV_DEF));

	if(check_drdy(disk) < 0)
		return -1;

	WRITE_REG(disk, ATA_STAT_COM_ADDR, ATA_CMD_IDN_DRV);


	if(check_drq(disk) < 0)
		return -1;

	for(index = 0; index < 256; index++)
		buf[index] = READ_REG_WORD(disk, ATA_DATA_REG_ADDR);

	return 0;
}


/* Convert ATA string to ascii string */
static inline void get_ascii(uint16_t *data, char *string, int length)
{
	int index, nindex = 0;

	for(index = 0; index < length; index++)
	{
		string[nindex++] = (char)(data[index] / 256);
		//nindex++;
		string[nindex++] = (char)(data[index] % 256);
		//nindex++;
	}
	string[length] = '\0';
}



/* Detect the available ata disks */
int disk_detect()
{
	int num, ret;
	struct ata_info *info;
	uint16_t ata_id[256];

	for(num = 0; num < 1; num++)
	{

		ata_soft_reset(num);
		ret = read_driveid(num, ata_id);
		if(ret < 0)
		{
		//	printf("ATA ERROR %x", ret);
		//	printf("ne disk %x. \n", ata_id[0]);
			continue;
		}

		info = &device[num].info;
		info->sector_size = ata_id[5];
		if(info->sector_size == 0)
			info->sector_size = 512;

		info->ata_type = ata_id[0];
		info->log_ncylinders = (uint32_t)ata_id[1];
		info->log_nheads = (uint32_t)ata_id[3];
		info->log_nsectors = (uint32_t)ata_id[6];
		info->nsectors = (uint32_t)((uint16_t)ata_id[57] | (((uint16_t)ata_id[58]) << 16));
		get_ascii(&ata_id[27], info->model, 20);
		get_ascii(&ata_id[10], info->serial, 10);
		get_ascii(&ata_id[23], info->firmware, 4);
		info->max_sect_transfer = ata_id[47] & 0xFF;
		info->dma_modes = ata_id[63];
		info->pio_modes = ata_id[64];
		info->udma_modes = ata_id[88];

		info->min_dma_cycletime = ata_id[65];
		info->def_dma_cycletime = ata_id[66];
		info->min_pio_cycletime = ata_id[67];
		info->min_pio_cycletime_IORDV = ata_id[68];

		info->ata_major = ata_id[80];
		info->ata_minor = ata_id[81];
		info->commands_supported = ata_id[82] | (ata_id[83] << 16);
		info->commands_status = ata_id[85] | (ata_id[86] << 16);

		if(info->nsectors == 0)
		{
			//printf("ne disk %x. \n", info->ata_type);
			continue;
		}

		printf("\n ATA Disk: %s %d MB ",info->model, info->nsectors/(2 * 1024));
		device->pinfo = initialize_partitions(DEVICE(1, num, 0), info->sector_size, ata_strategy);

	}
	
	return 0;
}


/* Read the disk directly */
int read_ata(struct operation *op, int sector_size)
{
	int reg, disk = DISKNO(op->device);
	int length;

	WRITE_REG(disk, ATA_SECT_COUNT_ADDR, op->blksize);

	write_lba(disk, op->block);
	WRITE_REG(disk, ATA_STAT_COM_ADDR, readcmd);
	if(READ_REG(disk, ATA_STAT_COM_ADDR) & ATA_STAT_ERR)
	{
		reg = READ_REG(disk, ATA_ERR_REG_ADDR);
		return -reg;
	}

	/* Now read the data */
	length = (op->blksize * sector_size);
	check_drq(disk);
	READ_DATA(disk, ATA_DATA_REG_ADDR, op->buffer, length);

	return 0;
}

int read_multi_lba(struct operation *ops, int sector_size)
{
	int disk = DISKNO(ops->device);

	if(disk > MAX_DISKS)
		return -EINVAL;
//	syscall_disable_interrupt();
	check_busy(disk);
	WRITE_REG(disk, ATA_SECT_COUNT_ADDR, ops->blksize);

	write_lba(disk, ops->block);

	check_drdy(disk);
	WRITE_REG(disk, ATA_STAT_COM_ADDR, readcmd );
//	syscall_enable_interrupt();
	return waitnread_data(ops, sector_size);
}

static inline int waitnwrite_data(struct operation *ops, int sector_size)
{
	int reg, disk = DISKNO(ops->device);
	int length;

//	syscall_sleep_unint(ops->caller);

	if(READ_REG(disk, ATA_STAT_COM_ADDR) & ATA_STAT_ERR)
	{
		reg = READ_REG(disk, ATA_ERR_REG_ADDR);
		return -reg;
	}

	check_drq(disk);
	/* Now read the data */
	length = (ops->blksize * sector_size);
	WRITE_DATA(disk, ATA_DATA_REG_ADDR, ops->buffer, length);

	return 0;
}

int write_multi_lba(struct operation *ops, int sector_size)
{
	int disk = DISKNO(ops->device);

	if(disk > MAX_DISKS)
		return -EINVAL;

	check_busy(disk);
	WRITE_REG(disk, ATA_SECT_COUNT_ADDR, ops->blksize);

	write_lba(disk, ops->block);

	check_drdy(disk);
	WRITE_REG(disk, ATA_STAT_COM_ADDR, writecmd);

	return waitnwrite_data(ops, sector_size);
}


int ata_strategy(struct operation *op)
{
	int diskno = DISKNO(op->device);
	int part;
	int sector_size = device[diskno].info.sector_size;
	struct partition_info *pinfo;

	part = PARTNO(op->device);

	op->blksize = op->blksize/sector_size;
	op->block = (op->block * op->blksize);
	pinfo = device[diskno].pinfo;
	/* If partition is 0, the disk is taken as a whole. */
	if(part != 0)
		op->block += pinfo[part].start;

//	printf("strategy for sects %d lbs %d pstart %d dsk %d ", op->blksize, op->block, pinfo[part].start, op->device);
	op->caller = pthread_self();

	if(diskno & 2)
		sec = op;
	else
		prim = op;

	if(op->type & BLK_READ)
		return read_multi_lba(op, sector_size);

	return write_multi_lba(op, sector_size);
}
