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
 * File: ata.h
 * 			AT Atachment device Declarations.
 */

#ifndef __FSIO_ATA_H
#define __FSIO_ATA_H

#include <asm/ipc.h>

#include "blkdev.h"
#include "atax86.h"
#include "svrconfig.h"

/* Register Addresses offset (Arch independent) */
#define ATA_DATA_REG_ADDR	0x00		/* Data Port Register (r/w) */

#define ATA_ERR_REG_ADDR	0x01		/* Error Register (r) */
										/* Write Precompensation (w) */

#define ATA_SECT_COUNT_ADDR	0x02		/* Sector Count	Register (r/w) */

#define ATA_SECT_NUM_ADDR	0x03		/* Sector Number Register (r/w) on CHS mode */
										/* LBA bit (0-7) on LBA mode */

#define ATA_CYL_LOW_ADDR	0x04		/* Cylinder Low  Register (r/w) on CHS mode */
										/* LBA bit (8-15) on LBA mode */

#define ATA_CYL_HIGH_ADDR	0x05		/* Cylinder High (r/w) on CHS mode */
										/* LBA bit (16-23) on LBA mode */

#define ATA_DEV_HEAD_ADDR	0x06		/* Device/Head number Register (r/w) on CHS mode */
										/* LBA bit (24-27) on LBA mode */

#define ATA_STAT_COM_ADDR	0x07		/* Status Register (r) */
										/* Command Register (w) */

/* Alternate Status Register and Status Register Bits Mask */
#define ATA_STAT_ERR		0x01		/* Error */
#define ATA_STAT_IDX		0x02		/* Index (vendor specific). */
#define ATA_STAT_CORR		0x04		/* Corrected Data */
#define ATA_STAT_DRQ		0x08		/* Data Request */
#define ATA_STAT_DSC		0x10		/* Device Seek Complete */
#define ATA_STAT_DF			0x20		/* Device Fault */
#define ATA_STAT_DRDY		0x40		/* Device Ready */
#define ATA_STAT_BSY		0x80		/* Busy */


/* Error Register Bit Mask	*/
#define ATA_ERR_DEF			0x80		/* Default Bits, Always set */
#define ATA_ERR_AMNF		0x01		/* Address Mark Not Found */
#define ATA_ERR_TK0NF		0x02		/* Track 0 Not Found */
#define ATA_ERR_ABRT		0x04		/* Aborted Command */
#define ATA_ERR_MCR			0x08		/* Media Change Requested */
#define ATA_ERR_IDNF		0x10		/* ID Not Found */
#define ATA_ERR_MC			0x20		/* Media Changed */
#define ATA_ERR_UNC			0x40		/* Uncorrectable Data Error */

/* Device Control Register Bit Mask */
#define ATA_DEVCTRL_DEF		0x08		/* Default Bits (Always Set) */
#define ATA_DEVCTRL_SRST	0x04		/* Software Reset Bit */
#define ATA_DEVCTRL_nIEN	0x02		/* Device Interrupt Enable */

/* Device/Head Register Bit Mask */
#define ATA_DEV_DEF			0xA0		/* Default Bits (Always Set) */
#define ATA_DEV_LBA			0x40		/* LBA mode Select */
#define ATA_DEV_DEV			0x10		/* Device Select. 0=device 0, 1=device 1 */

/* ATA Commands */
#define ATA_CMD_NOP			0x00		/* NO operation */
#define ATA_CMD_FMT_TRK		0x50		/* Format Track */
#define ATA_CMD_DRV_DIAG	0x90		/* Execute Drive Diagonistic */
#define ATA_CMD_INIT_DRV	0x91		/* Initialize Drive Parameters */
#define ATA_CMD_CHKPOWER	0x98		/* Check Power Mode  */
#define ATA_CMD_PACKET		0xA0		/* Packet command for atapi */
#define ATA_CMD_READ_MULTI	0xC4		/* Read Multiple Sectors in single Interrupt */
#define ATA_CMD_WRITE_MULTI	0xC5		/* Write Sectors in single interrupt */
#define ATA_CMD_READ_DMA	0xC8		/* Read Sector using DMA */

#define ATA_CMD_IDLE		0xE3		/* Idle */
#define ATA_CMD_READ_BUF	0xE4		/* Read the buffer */
#define ATA_CMD_CHKPOWER1	0xE5		/* Check Power Mode  (same as Above) */
#define ATA_CMD_IDN_DRV		0xEC		/* Identify Drive */
#define ATA_CMD_MEDIA_EJECT	0xED		/* Eject the media */
#define ATA_CMD_MEDIA_LOCK	0xDE		/* Lock the media */
#define ATA_CMD_MEDIA_UNLOCK	0xDF	/* UnLock the media */
#define ATA_CMD_READ_SECT	0x20		/* Read the Sectors */
#define ATA_CMD_WRITE_SECT	0x21		/* Write the Sectors */
#define ATA_CMD_READ_VERIFY_SECT 0x40	/* Read verify the sectors */
#define ATA_CMD_SEEK		0x70		/* Dive Seek */
#define ATA_CMD_SET_FEAT	0xEF		/* Set the Features */

#define MAX_DISKS			4

/* ATA disk information */
struct ata_info {
	uint16_t sector_size;
	uint16_t	ata_type;
	uint16_t	log_ncylinders;
	uint16_t	log_nheads;
	uint16_t	log_nsectors;
	uint32_t	nsectors;
	char 	serial[20];
	char 	firmware[8];
	char 	model[40];
	uint16_t 	max_sect_transfer;
//	int 		capabilities;
	uint16_t dma_modes;
	uint16_t pio_modes;
	uint16_t udma_modes;

	uint16_t min_dma_cycletime;
	uint16_t def_dma_cycletime;
	uint16_t min_pio_cycletime;
	uint16_t min_pio_cycletime_IORDV;

	uint16_t ata_major;
	uint16_t ata_minor;
	uint32_t commands_supported;
	uint32_t commands_status;
};

struct diskinfo {
	struct ata_info info;
	struct partition_info *pinfo;
};

#ifndef SEPERATE_IO_PROCESS
 extern int ata_open(struct operation *op);
 extern int ata_close(struct operation *op); 
 extern int ata_size(struct operation *op);
#else 
 extern int ata_open(message_t );
 extern int ata_close(message_t ); 
 extern int ata_size(message_t );
#endif
 
extern int ata_read(message_t );
extern int ata_write(message_t );

extern int ata_strategy(struct operation *);
extern int request_ionamespace();

extern void ata_soft_reset(unsigned char );
extern void ata_intr_primary();
extern void ata_intr_secondary();
extern void *ata_thr(void *);
extern int ata_init();
extern int ata_exit();
extern int disk_detect();
extern int read_ata(struct operation *, int );
extern int read_multi_lba(struct operation *, int );
extern int write_multi_lba(struct operation *, int );


#endif	/* __FSIO_ATA_H  */
