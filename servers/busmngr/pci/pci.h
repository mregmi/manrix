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
 * File: pci.h
 * 		Peripheral Component Interconnect (PCI) driver declaration.
 */

#ifndef __PCI_H
#define __PCI_H

#include <stdint.h>
#include <sys/types.h>

#include "bios32.h"

/* PCI signature "PCI " */
#define PCI_SIGNATURE           (('P') + ('C' << 8) + ('I' << 16) + (' ' << 24))

/* PCI BIOS ID for PCI (Component Id): "$PCI" */
#define PCI_ID					(('$') + ('P' << 8) + ('C' << 16) + ('I' << 24))

/* PCI Functions */
#define PCI_FUNCTION_ID			(0xB100)
#define PCI_BIOS_PRESENT 		(0x01 | PCI_FUNCTION_ID)
#define FIND_PCI_DEVICE 		(0x02 | PCI_FUNCTION_ID)
#define FIND_PCI_CLASS_CODE 	(0x03 | PCI_FUNCTION_ID)
#define GENERATE_SPECIAL_CYCLE	(0x06 | PCI_FUNCTION_ID)
#define READ_CONFIG_BYTE 		(0x08 | PCI_FUNCTION_ID)
#define READ_CONFIG_WORD 		(0x09 | PCI_FUNCTION_ID)
#define READ_CONFIG_DWORD 		(0x0A | PCI_FUNCTION_ID)
#define WRITE_CONFIG_BYTE 		(0x0B | PCI_FUNCTION_ID)
#define WRITE_CONFIG_WORD 		(0x0C | PCI_FUNCTION_ID)
#define WRITE_CONFIG_DWORD 		(0x0D | PCI_FUNCTION_ID)

/* Return Codes */
#define SUCCESSFUL 				0x00
#define FUNC_NOT_SUPPORTED 		0x81
#define BAD_VENDOR_ID 			0x83
#define DEVICE_NOT_FOUND 		0x86
#define BAD_REGISTER_NUMBER 		0x87

/* PCI Base Class Codes */
#define PCI_CLASS_UNKNOWN		0x0000	/* Devices that were in Being Before Classes */
#define PCI_CLASS_STORAGE		0x0100	/* Mass Storage Devices */
#define PCI_CLASS_NETWORK		0x0200	/* Network Devices */
#define PCI_CLASS_DISPLAY		0x0300	/* Display Adaptors */
#define PCI_CLASS_MULTIMEDIA	0x0400	/* Multimedia Devices */
#define PCI_CLASS_MEMORY		0x0500	/* Memory Devices */
#define PCI_CLASS_BRIDGE		0x0600	/* Bridges */
#define PCI_CLASS_SIMPLE_COMMN	0x0700	/* Simple Communications Controller */
#define PCI_CLASS_BSP			0x0800	/* Base System Peripherals class */
#define PCI_CLASS_INPUT			0x0900	/* Input Device Class */
#define PCI_CLASS_DOCK			0x0A00	/* Docking Stations */
#define PCI_CLASS_PROCESSOR		0x0B00	/* Processors */
#define PCI_CLASS_SERIAL_BUS	0x0C00	/* Serial BUS */
#define PCI_CLASS_WIRELESS		0x0D00	/* Wireless Controller */
#define PCI_CLASS_I20			0x0E00	/* Intelligent controller */
#define PCI_CLASS_SATCOM		0x0F00	/* Satellite communications controller */
#define PCI_CLASS_ENCRYPT		0x1000	/* Encryption Controller */
#define PCI_CLASS_SIGP			0x1100	/* Signal Processing Controller */
#define PCI_CLASS_FF			0xFF00	/* That Does not fit in any of above */

/* PCI subclass values */
/* UNKNOWN Class */
#define PCI_UNKNOWN_ALL			0x00	/* All Except VGA */
#define PCI_UNKNOWN_VGA			0x01	/* VGA Devices */

/* Mass Storage Device Class */
#define PCI_STORAGE_SCSI		0x00	/* SCSI Controller */
#define PCI_STORAGE_IDE			0x01	/* IDE Controller */
#define PCI_STORAGE_FDC			0x02	/* SCSI Controller */
#define PCI_STORAGE_IPI			0x03	/* IPI Controller ??? */
#define PCI_STORAGE_RAID		0x04	/* RAID Controller */
#define PCI_STORAGE_OTHER		0x80	/* Other Mass Storage Device */

/* Network Controller Class */
#define PCI_NETWORK_ETHERNET		0x00	/* Ethernet Controller */
#define PCI_NETWORK_TOKEN_RING		0x01	/* Token Ring Controller */
#define PCI_NETWORK_FDDI		0x02	/* FDDI Controller */
#define PCI_NETWORK_ATM			0x03	/* ATM Controller */
#define PCI_NETWORK_ISDN		0x04	/* ISDN Controller */
#define PCI_NETWORK_OTHER		0x80	/* Other Network Controller */

/* Display Adaptor Class */
#define PCI_DISPLAY_VGA			0x00	/* VGA Controller */
#define PCI_DISPLAY_XGA			0x01	/* XGA Controller */
#define PCI_DISPLAY_3D			0x02	/* 3D Controller */
#define PCI_DISPLAY_OTHER		0x80	/* Other Display Controller */

/* Multimedia Device Class */
#define PCI_MULTIMEDIA_VIDEO		0x00	/* Multimedia Video Controller */
#define PCI_MULTIMEDIA_AUDIO		0x01	/* Multimedia Audio Controller */
#define PCI_MULTIMEDIA_TELEPHONY	0x02	/* Computer Telephony Controller */
#define PCI_MULTIMEDIA_OTHER		0x80	/* Other Multimedia Controller */

/* Memory Controllers */
#define PCI_MEMORY_RAM			0x00	/* RAM Controller */
#define PCI_MEMORY_FLASH		0x01	/* FLASH Memory Controller */
#define PCI_MEMORY_OTHER		0x80	/* Other Memory Controller */

/* Bridges */
#define PCI_BRIDGE_HOST			0x00	/* Host Bridge */
#define PCI_BRIDGE_ISA			0x01	/* ISA Bridge */
#define PCI_BRIDGE_EISA			0x02	/* EISA Bridge */
#define PCI_BRIDGE_MCA			0x03	/* MCA Bridge */
#define PCI_BRIDGE_PCI			0x04	/* PCI to PCI Bridge */
#define PCI_BRIDGE_PCMCIA		0x05	/* PCMCIA Bridge */
#define PCI_BRIDGE_NUBUS		0x06	/* NuBus Bridge */
#define PCI_BRIDGE_CARDBUS		0x07	/* CardBus Bridge */
#define PCI_BRIDGE_RACEWAY		0x08	/* RACEway Bridge */
#define PCI_BRIDGE_P2P			0x09	/* Semi-transparent PCI-to-PCI bridge */
#define PCI_BRIDGE_I2P			0x0A	/* InfiniBand to PCI host bridge */
#define PCI_BRIDGE_OTHER		0x80	/* Other Bridges */

/* Simple Communications Controller */
#define PCI_SIMPLE_COMMN_SERIAL		0x00	/* Serial Simple Communications Controller  */
#define PCI_SIMPLE_COMMN_PARALLEL	0x01	/* Parallel Simple Communications Controller  */
#define PCI_SIMPLE_COMMN_MULTIPS	0x02	/* Multiport serial controller */
#define PCI_SIMPLE_COMMN_MODEM		0x03	/* Modem */
#define PCI_SIMPLE_COMMN_OTHER		0x80	/* Other Simple Communications Controller  */

/* Basic System Peripherals */
#define PCI_BSP_PIC			0x00	/* Programmable Interrupt Controller */
#define PCI_BSP_DMA			0x01	/* DMA Controller */
#define PCI_BSP_TIMER		0x02	/* System Timer */
#define PCI_BSP_RTC			0x03	/* Real Time Chip Controller */
#define PCI_BSP_PCI_HOT		0x04	/* PCI HotPlug Controller */
#define PCI_BSP_OTHER		0x80	/* Other Controllers */

/* Input Device Class */
#define PCI_INPUT_KEYBOARD		0x00	/* Keyboard Controller */
#define PCI_INPUT_DIGITIZER		0x01	/* Digitizers (pen) */
#define PCI_INPUT_MOUSE			0x02	/* Mouse Controller */
#define PCI_INPUT_SCANNER		0x03	/* Scanner Controller */
#define PCI_INPUT_GAMEPORT		0x04	/* GamePort Controller */
#define PCI_INPUT_OTHER			0x80	/* Other Input Controllers */

/* Docking Stations */
#define PCI_DOCK_GENERIC		0x00	/* Generic Docking Stations */
#define PCI_DOCK_OTHER			0x80	/* Other Docking Stations */

/* Processor Classes */
#define PCI_PROCESSOR_386		0x00	/* 386 Based Processors */
#define PCI_PROCESSOR_486		0x01	/* 486 Based Processors */
#define PCI_PROCESSOR_PENTIUM		0x02	/* Pentium Based Processors */
#define PCI_PROCESSOR_ALPHA		0x10	/* Alpha Based Processors */
#define PCI_PROCESSOR_PPC		0x20	/* PowerPC Based Processors */
#define PCI_PROCESSOR_MIPS		0x30	/* MIPS Based Processors */
#define PCI_PROCESSOR_COP		0x40	/* Co-Processors */

/* Serial Bus Controllers */
#define PCI_SERIAL_FIREWIRE		0x00	/* FireWire (IEEE 1394) */
#define PCI_SERIAL_ACCESS		0x01	/* ACCESS bus */
#define PCI_SERIAL_SSA			0x02	/* SSA bus */
#define PCI_SERIAL_USB			0x03	/* USB */
#define PCI_SERIAL_FIBER		0x04	/* Fiber Channel */
#define PCI_SERIAL_SMBUS		0x05	/* SMBus Controller */
#define PCI_SERIAL_INIFINI		0x06	/* Inifini Band */

/* Wireless controller */
#define PCI_WIRELESS_IRDA		0x00  	/* IRDA controller */
#define PCI_WIRELESS_CIR		0x01  	/* Consumer IR controller */
#define PCI_WIRELESS_RF			0x10  	/* RF controller */

/* Intelligent controller */
#define PCI_I2O					0x00	/*  I2O */

/* Satellite communications controller */
#define PCI_SATCOM_TV			0x00 	/* Satellite TV controller */
#define PCI_SATCOM_AUDIO		0x01  	/* Satellite audio communication controller */
#define PCI_SATCOM_VOICE		0x03  	/* Satellite voice communication controller */
#define PCI_SATCOM_DATA			0x04  	/* Satellite data communication controller */

/* Encryption controller */
#define PCI_ENCRYPT_NET			0x00  	/* Network and computing encryption device */
#define PCI_ENCRYPT_ENT			0x10  	/* Entertainment encryption device */

/* Signal processing controller */
#define PCI_SIGP_DPIO			0x00  	/* DPIO module */
#define PCI_SIGP_PC				0x01  	/* Performance counters */
#define PCI_SIGP_COMN			0x10  	/* Communication synchronizer */

/*
 * Structure to hold PCI subclass  name and
 * max no of lclass it contains
 */
struct pci_subclass{
	uint8_t max_lclass;
	const char *name;
};

/*
 * Structure to hold PCI class  name and
 * max no of subclass it contains
 */
struct pci_class{
	uint8_t max_subclass;
	const char *name;
	struct pci_subclass *subclass;
};

struct pciid{
	char type;
	unsigned short id;
	const char *string;
};

extern int PCIBIOS_ispresent(struct bios32_service *);

extern int find_PCI_device(struct bios32_service *,
		uint16_t , uint16_t , uint16_t ,
		uint8_t *, uint8_t *);

extern int find_PCI_class_code(struct bios32_service *pcisrv,
		uint32_t classcode, uint16_t index,
		uint8_t *bus, uint8_t *dev);

extern int generate_special_cycle(struct bios32_service *pcisrv,
		uint8_t bus, uint32_t data);

extern int read_configuration_byte(struct bios32_service *pcisrv,
		uint8_t bus, uint8_t device, uint8_t reg, uint8_t *data);

extern int read_configuration_word(struct bios32_service *pcisrv,
		uint8_t bus, uint8_t device, uint8_t reg, uint16_t *data);

extern int read_configuration_dword(struct bios32_service *pcisrv,
		uint8_t bus, uint8_t device, uint8_t reg, uint32_t *data);

extern int write_configuration_byte(struct bios32_service *pcisrv,
		uint8_t bus, uint8_t device, uint8_t reg, uint8_t data);

extern int write_configuration_word(struct bios32_service *pcisrv,
		uint8_t bus, uint8_t device, uint8_t reg, uint16_t data);

extern int write_configuration_dword(struct bios32_service *pcisrv,
		uint8_t bus, uint8_t device, uint8_t reg, uint32_t data);

#endif /* __PCI_H */

