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
 */
/*
 * File: fdc.c
 *	 The Floppy Disc Controller (FDC) driver.
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
#include <asm/vm.h>

#include "fdc.h"
#include "blkdev.h"

#ifndef SEPERATE_IO_PROCESS
struct block_operations fdcblkop =	{
			.d_open = fdc_open,
			.d_close = fdc_close,
			.d_strategy = fdc_strategy,
			.d_size = fdc_size
		};

#endif /* SEPERATE_IO_PROCESS */

static struct device *device;
static struct operation *glob_oper;
static void *memaddr;		/* Low Memory address for DMA transfer */

/* Wait till interrupt arrives; */
static inline void wait_for_int()
{
	int i;
//	if(glob_oper)
//		syscall_sleep_unint(glob_oper->caller);
	for(i=0;i<1000000;i++)
		asm("nop");
}

/*
 * A routine to send a Command or Parameter Byte to the FDC Controler.
 */
int send_byte(uint8_t val)
{
	int timeout = 100000;
	unsigned char data;

	do{
		data = READ_REG_BYTE(FDC_REG_MSR);
		if(!--timeout)
			return -1;
	}while((data & MSR_MRQ) != MSR_MRQ);

	WRITE_REG_BYTE(FDC_REG_DATA, val);

	return 0;
}

/*
 * A routine to Get a Byte from the FDC Controler.
 */
int get_byte()
{
	int timeout = 100000;
	unsigned char data, stat;

	do{
		stat = READ_REG_BYTE(FDC_REG_MSR);
		if(!--timeout)
			return -1;
	}while((stat & (MSR_MRQ|MSR_DIO)) != (MSR_MRQ|MSR_DIO));

	data = READ_REG_BYTE(FDC_REG_DATA);

	return (int)data;
}

/*
 * Reset the FDC.
 */
void fdc_reset()
{
	WRITE_REG_BYTE(FDC_REG_DOR, 0);
}

/* Turn on the Motor */
static inline void enable_motor(unsigned char drive)
{
	unsigned char val;

	/* Enable Drive and Motor */
	val = 1 << drive;
	val <<= 4;					/* Which Motor */
	val |= drive|DOR_CTR_ENABLE|DOR_DMA_IRQ;	/* DMA and ctrlr */
	WRITE_REG_BYTE(FDC_REG_DOR, val);
}

/* Turn off the Motor */
static inline void disable_motor()
{
	unsigned char val;

	/* Disable Drive and Motor */
	val = DOR_CTR_ENABLE|DOR_DMA_IRQ;	/* DMA and ctrlr */
	WRITE_REG_BYTE(FDC_REG_DOR, val);
}


/* Recalibrate the FDC */
void recalibrate(uint8_t drive)
{
//	unsigned char st0;

	/* Enable Drive and Motor */
	enable_motor(drive);

	/* send Calibrate Command */
	send_byte(CMD_CALIBRATE);
	send_byte(drive);

	/* wait for interrupt */
	wait_for_int();

	send_byte(CMD_SENSE_INT);
}

/*
 * Initialize the FDC controller.
 */
int fdc_initialize()
{
	/* reset controller */
	fdc_reset();

	/* set data reate via CCR: 500Kbps */
	WRITE_REG_BYTE(FDC_REG_CCR, 0);

	/* Enable controller and DMA */
	WRITE_REG_BYTE(FDC_REG_DOR, DOR_CTR_ENABLE|DOR_DMA_IRQ);

	return 0;
}

/* Get the results after a command */
static inline void get_results(uint8_t results[])
{
	int index = 0, timeout = 100000;
	while(index < 7)
	{
		results[index++] = get_byte();
		while(READ_REG_BYTE(FDC_REG_MSR) & MSR_BUSY)
		if(--timeout)
		{
			timeout = 100000;
			break;
		}
	}
	printf("results over");
}

/* FDC interrupt Handler */
void fdc_intr()
{
//	printf("Interrupt Arrived \n");
//	if(glob_oper != NULL)
//		syscall_wake_unint(glob_oper->caller);

//	finish_intr();
}


/*void *fdc_thr(void *v)
{
	syscall_change_iopl(3);
	syscall_IRQ_request(IRQ_FDC, fdc_intr);

	IRQ_init(IRQ_FDC);


	do{

		syscall_IRQ_wait();

	}while(1 == 1);

	return NULL;
}
*/
/*
 * Detect no of devices and its parameters ???
 */
static inline void detect_devices(struct device *dev)
{
	int index;

	for(index = 0; index < 4; index++)
	{
		dev[index].sector_size = 512;
	}
}

/* Initialize FDC driver */
int fdc_init()
{
	struct vn_object obj;

	printf("Initializing Floppy Driver... \n");
	IRQ_request(IRQ_FDC, fdc_intr);
	device = malloc(sizeof(struct device) * 4);
	if(!device)
		return -1;

	detect_devices(device);

#ifdef SEPERATE_IO_PROCESS
	request_ionamespace();
#else
	register_blockdev(2, &fdcblkop, 4);
#endif

	obj.size = 512;
	memaddr = (void *)syscall_vm_map(&obj, VM_MAP_LOWMEM);
	if(memaddr == NULL)
	{
		printf("Could Not allocate space in lowmem\n");
		return -1;
	}
	return 0;
}

/* exit fdc driver */
int fdc_exit()
{
	free(device);
#ifndef SEPERATE_IO_PROCESS
	unregister_blockdev(2);
#endif
	return 0;
}

/* Open an FDC device */
#ifndef SEPERATE_IO_PROCESS
int fdc_open(struct operation *op)
{
	return 0;
}

/* close an FDC device */
int fdc_close(struct operation *op)
{
	return 0;
}

int fdc_size(struct operation *op)
{
	//return disk size;
	return 0;
}
#endif

/* Convert LBA to CHS */
static inline void lba_to_chs(uint32_t lba, uint8_t *head, uint8_t *cyl, uint8_t *sector)
{
	int temp;

	*cyl = lba / (HEADS_PER_CYL_144 * SECTS_PER_TRACK_144);
	temp = lba % (HEADS_PER_CYL_144 * SECTS_PER_TRACK_144);
	*head = temp / SECTS_PER_TRACK_144;
	*sector = temp % SECTS_PER_TRACK_144 + 1;
}

/*
 * Read a sector from FDC
 */
static inline int fdc_rw_sect(uint32_t block, uint8_t drive, int type, void *data)
{
	unsigned char head, cyl, sector;
	unsigned char results[7];
	int times;


	/* Enable Drive and motor */
	enable_motor(drive);

	/* set data reate via CCR: 500Kbps */
	WRITE_REG_BYTE(FDC_REG_CCR, 0);

//	recalibrate(drive);
	for(times = 0; times < 3; times++)
	{
		/* Initialize DMA for DMA transfer (see fdcx86.h ) */
		dma_init(memaddr, 512, type);

		/* Convert LBA to CHS */
		lba_to_chs(block, &head, &cyl, &sector);

		if(type & BLK_READ)
			send_byte(CMD_READ_DATA);
		else
			send_byte(CMD_WRITE_DATA);

		send_byte((head << 2) | drive);
		send_byte(cyl);
		send_byte(head);
		send_byte(sector);
		send_byte(2);	/* 512 bytes per sector */
		send_byte(SECTS_PER_TRACK_144);
		send_byte(GAP3_RW_144);

		printf("Waiting for int");
		wait_for_int();
		printf("Int Arrived");
		get_results(results);
		printf("Results Arrived");
//		if(results[RESULT_ST0] & 0xC0)
//			continue;
		
		memcpy(data, memaddr, 512);
		return 0;
	}

	return -1;
}

/*
 * FDC read/write Operation
 */
int fdc_rw(struct operation *op, int sector_size)
{
	int times, ret;
	void *data = op->buffer;
	uint32_t block = op->block;
	uint8_t device = PARTNO(op->device);

	for(times = 0; times < op->blksize; times++)
	{
		ret = fdc_rw_sect(block, device, op->type, data);
		if(ret < 0)
			return ret;
		printf("DATA %s", data);
//		for(;;);
		block++;
		data = (void *)((uint32_t)data + sector_size);
	}

	return 0;
}

/*
 * Strategy Routine for FDC.
 */
int fdc_strategy(struct operation *op)
{
	/*
	 * 0 = fd0, 1 = fd1 etc
	 */
	int diskno = PARTNO(op->device);
	int sector_size = device[diskno].sector_size;

	op->blksize = op->blksize/sector_size;
	op->block = (op->block * op->blksize);
	op->caller = pthread_self();

	glob_oper = op;
	printf("reading block %d \n", op->block);
	return fdc_rw(op, sector_size);
}
