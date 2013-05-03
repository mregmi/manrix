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
 * File: fdc.h
 * 	Floppy Disc Controller (FDC) Declarations.
 */

#ifndef __FSIO_FDC_H
#define __FSIO_FDC_H

#include <asm/ipc.h>

#include "blkdev.h"
#include "fdcx86.h"
#include "svrconfig.h"

/* FDC registers */
#define FDC_REG_DOR		0x02	/* Digital Output Register (DOR) */
#define FDC_REG_MSR		0x04	/* Main Status Register */
#define FDC_REG_ST0		0x00	/* Status Register 0 */
#define FDC_REG_DATA		0x05	/* Data Register */
#define FDC_REG_DIR		0x07	/* Digital Input Register */
#define FDC_REG_CCR		0x07	/* Configuration Control Register */

/* Digital Output Register (DOR) bitmasks */
#define DOR_DRIVE0		0x00	/* Drive 0 (A) Select */
#define DOR_DRIVE1		0x01	/* Drive 1 (B) Select */
#define DOR_DRIVE2		0x02	/* Drive 2 (C) Select */
#define DOR_DRIVE3		0x03	/* Drive 3 (D) Select */
#define DOR_CTR_ENABLE		0x04	/* Controller Enable (1); Controller Reset (0) */
#define DOR_DMA_IRQ		0x08	/* 1 = Enable DMA and IRQ channel */
#define DOR_MOTOA		0x10	/* Motor A Select; 1 = Start Motor*/
#define DOR_MOTOB		0x20	/* Motor B Select; 1 = Start Motor*/
#define DOR_MOTOC		0x40	/* Motor C Select; 1 = Start Motor*/
#define DOR_MOTOD		0x80	/* Motor D Select; 1 = Start Motor*/

/* Main Status Register (MSR) Bitmasks */
#define MSR_ACTA		0x01	/* Drive A active */
#define MSR_ACTB		0x02	/* Drive B active */
#define MSR_ACTC		0x04	/* Drive C active */
#define MSR_ACTD		0x08	/* Drive D active */
#define MSR_BUSY		0x10	/* Controller is Busy */
#define MSR_NDMA		0x20	/* Controller Not in DMA mode */
#define MSR_DIO			0x40	/* Data Input Output; 1 = ctrlr->cpu; 0 = cpu->ctrlr */
#define MSR_MRQ			0x80	/* Data Register Ready */

/* Digital Input Register (DIR) Bitmasks */
#define DIR_CHAN		0x80	/* Disk Change 1 = disk changed since last command */

/* Control Configuration Register (CCR) Bitmasks */
#define CCR_RAT0		0x01	/* Data Transfer Rate; 00 = 500kbits/s, 01 = 300kbits/s */
#define CCR_RAT1		0x02	/* 10 = 250kbits/s */
#define CCR_RAT2		0x03	/* 11 - 1Mbits/s */

/* Command Control Bits */
#define CTRL_MFM		0x40	/* MFM mode Selector; 1 = Double Density */
#define CTRL_MT			0x80	/* Multi Track Selector */
#define CTRL_SK			0x20	/* Skip Flag */

/* FDC Commands */
#define CMD_READ_DATA		(0x06 | CTRL_MFM | CTRL_MT | CTRL_SK)
#define CMD_WRITE_DATA		(0x05 | CTRL_MFM | CTRL_MT | CTRL_SK)
#define CMD_CALIBRATE		(0x07)
#define CMD_SENSE_INT		0x08
#define CMD_SEEK			0x0F

/* Floppy PARAMS */
#define HEADS_PER_CYL_144	2
#define SECTS_PER_TRACK_144	18

#define GAP3_RW_144		0x1B

#define RESULT_ST0		0
#define RESULT_ST1		1
#define RESULT_ST2		2
#define RESULT_CYL		3
#define RESULT_HEAD		4
#define RESULT_SECT		5
#define RESULT_SECTSZ	6

struct device{
	int sector_size;
};

extern int fdc_open(struct operation *);
extern int fdc_close(struct operation *);
extern int fdc_size(struct operation *);

extern int send_byte(uint8_t );
extern int get_byte();
extern void fdc_reset();
extern void recalibrate(uint8_t );
extern int fdc_initialize();
extern void fdc_intr();
extern void *fdc_thr(void *);
extern int fdc_init();
extern int fdc_exit();
extern int fdc_strategy(struct operation *);
extern int fdc_rw(struct operation *, int );

#endif	/* __FSIO_FDC_H  */
