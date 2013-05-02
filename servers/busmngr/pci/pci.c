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
 * File: pci.c
 * 		Peripheral Component Interconnect (PCI) driver.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bios32.h"
#include "pci.h"


struct bios32_service pci;


/* PCI subclasses */
/* UNKNOWN Class */
struct pci_subclass PCI_unknown[] = {
	{ 0, "Non VGA"},
	{ 0, "VGA Devices"}
};

/* Mass Storage Device Class */
struct pci_subclass PCI_storage[] = {
	{ 0, "SCSI Controller"},
	{ 0, "IDE Controller"},
	{ 0, "FDC Controller"},
	{ 0, "IPI Controller"},
	{ 0, "RAID Controller"},
};

/* Network Controller Class */
struct pci_subclass PCI_network[] = {
	{ 0, "Ethernet Controller"},
	{ 0, "Token Ring Controller"},
	{ 0, "FDDI Controller"},
	{ 0, "ATM Controller"},
	{ 0, "ISDN controller"},
};

/* Display Adaptor Class */
struct pci_subclass PCI_display[] = {
	{ 0, "VGA Controller"},
	{ 0, "XGA Controller"},
	{ 0, "3D controller"},
};

/* Multimedia Device Class */
struct pci_subclass PCI_multimedia[] = {
	{ 0, "Multimedia Video Controller"},
	{ 0, "Multimedia Audio Controller"},
	{ 0, "Computer telephony device"},
};

/* Memory Controllers */
struct pci_subclass PCI_memory[] = {
	{ 0, "RAM Controller"},
	{ 0, "FLASH Memory Controller"},
};

/* Bridges */
struct pci_subclass PCI_bridge[] = {
	{ 0, "Host Bridge"},
	{ 0, "ISA Bridge"},
	{ 0, "EISA Bridge"},
	{ 0, "MCA Bridge"},
	{ 0, "PCI to PCI Bridge"},
	{ 0, "PCMCIA Bridge"},
	{ 0, "NuBus Bridge"},
	{ 0, "CardBus Bridge"},
	{ 0, "RACEway bridge"},
	{ 0, "Semi-transparent PCI-to-PCI bridge"},
	{ 0, "InfiniBand to PCI host bridge"},
};

/* Simple Communications Controller */
struct pci_subclass PCI_scc[] = {
	{ 0, "Serial Simple Communications Controller"},
	{ 0, "Parallel Simple Communications Controller"},
	{ 0, "Multiport serial controller"},
	{ 0, "Modem"},
};

/* Basic System Peripherals */
struct pci_subclass PCI_bsp[] = {
	{ 0, "Programmable Interrupt Controller"},
	{ 0, "DMA Controller"},
	{ 0, "System Timer"},
	{ 0, "Real Time Chip Controller"},
	{ 0, "Multiport serial controller"},
};

/* Input Device Class */
struct pci_subclass PCI_input[] = {
	{ 0, "Keyboard Controller"},
	{ 0, "Digitizers (pen)"},
	{ 0, "Mouse Controller"},
	{ 0, "Scanner controller"},
	{ 0, "Gameport controller"},
};


/* Docking Stations */
struct pci_subclass PCI_dock[] = {
	{ 0, "Generic Docking Stations"},
};

/* Processor Classes */
struct pci_subclass PCI_processor[] = {
	{ 0, "386 Based Processors"},
	{ 0, "486 Based Processors"},
	{ 0, "Pentium Based Processors"},
	{ 0, "Alpha Based Processors"},
	{ 0, "PowerPC Based Processors"},
	{ 0, "MIPS"},
	{ 0, "Co-Processor"},
};


/* Serial Bus Controllers */
struct pci_subclass PCI_serialbus[] = {
	{ 0, "FireWire (IEEE 1394)"},
	{ 0, "ACCESS bus"},
	{ 0, "SSA bus"},
	{ 0, "USB Controller"},
	{ 0, "Fiber Channel"},
	{ 0, "SMBus Controller"},
	{ 0, "InfiniBand"},
};

/* Wireless controllers */
struct pci_subclass PCI_wireless[] = {
	{ 0, "IRDA controller"},
	{ 0, "Consumer IR controller"},
	{ 0, "RF controller"},
};

/* Intelligent controller */
struct pci_subclass PCI_i2o[] = {
	{ 0, "I2O Controller"},
};

/* Satellite communications controller */
struct pci_subclass PCI_satcom[] = {
	{ 0, "Satellite TV controller"},
	{ 0, "Satellite audio communication controller"},
	{ 0, "Satellite voice communication controller"},
	{ 0, "Satellite data communication controller"},
};

/* Encryption controller */
struct pci_subclass PCI_encrypt[] = {
	{ 0, "Network and computing encryption device"},
	{ 0, "Entertainment encryption device"},
};

/* Signal processing controller */
struct pci_subclass PCI_sigp[] = {
	{ 0, "DPIO module"},
	{ 0, "Performance counters"},
	{ 0, "Communication synchronizer"},
};

/* PCI Class */
struct pci_class PCI_class[] = {
	{ 2, "UNKNOWN Device", PCI_unknown },
	{ 4, "Mass Storage Devices", PCI_storage },
	{ 3, "Network Devices", PCI_network },
	{ 1, "Display Adaptors", PCI_display },
	{ 1, "Multimedia Devices", PCI_multimedia },
	{ 1, "Memory Devices", PCI_memory },
	{ 7, "Bridges", PCI_bridge},
	{ 1, "Simple Communications Controller", PCI_scc },
	{ 3, "Base System Peripherals", PCI_bsp },
	{ 2, "Input Device", PCI_input },
	{ 0, "Docking Stations", PCI_dock },
	{ 0x40, "Processors", PCI_processor },
	{ 5,"Serial BUS Controllers", PCI_serialbus },
	{ 2, "Wireless controllers", PCI_wireless},
	{ 0, "Intelligent controller", PCI_i2o},
	{ 3, "Satellite communications controller", PCI_satcom},
	{ 1, "Encryption controller", PCI_encrypt},
	{ 2, "Signal processing controller", PCI_sigp},
	{ 0, NULL, NULL }
};

extern struct pciid vendors[];
//extern struct pciid devices[];

char *get_vendor_name(uint16_t vendor, int *off)
{
	char *name, type;
	uint16_t id;
	int index = 0;

	do{
		id = vendors[index].id;
		name = vendors[index].string;
		type = vendors[index].type;
		if((id == vendor) && (type == 'V'))
		{
			*off = index+1;
			return name;
		}
		index++;
	}while(name != NULL);

	return NULL;
}

char *get_device_name(uint16_t device, int off)
{
	char *name, type;
	uint16_t id;
	int index = off;

	do{
		id = vendors[index].id;
		name = vendors[index].string;
		type = vendors[index].type;
		if((id == device) && (type == 'D'))
			return name;
		if(type == 'V')
			break;
		index++;
	}while(name != NULL);

	return NULL;
}


int search_pci_devices(struct bios32_service *pcis)
{
	int cl, _class, index, ret;
	uint8_t bus, device, subclass, base;
	uint32_t ddata;
	int off;
	char *vname, *dname;

	printf("Detecting Deveices attached to PCI BUS.\n");
	for(cl = 0; cl < 0x11ff; cl++)
	{
		_class = (cl << 8) & 0xFFFF00;
		base = (cl >> 8) & 0xFF;

		for(index = 0; index < 0xFFFF; index++)
		{
			ret = find_PCI_class_code(pcis, _class, index, &bus, &device);
			if(ret != SUCCESSFUL)
			{
//				if(ret == DEVICE_NOT_FOUND)
					break;
//				else
//					continue;
			}

			ret = read_configuration_dword(pcis, bus, device, 0, &ddata);
			if(ret != SUCCESSFUL)
				continue;

			subclass = cl & 0xFF;
			if(subclass == 0x80)
				printf(" Other %s:", PCI_class[base].name);
			else
				printf(" %s ",PCI_class[base].subclass[subclass].name);

			vname = get_vendor_name((ddata & 0xFFFF), &off);
			if(vname == NULL)
				vname = "Unknown Vendor";

			dname = get_device_name(((ddata >> 16) & 0xFFFF), off);
			if(dname == NULL)
				dname = "Unknown Device";

			printf("%s by %s \n", dname, vname);
//			printf("on BUS=%d, Device=%d\n", bus, device);
		}
	}

	printf("PCI BUS search Over\n");
}

/*
 * Initialize the pci device Driver.
 */
int pci_init()
{
	struct bios32 bios32_header, *hptr;
	int ret, offset;

	/* Map the ROM so they are addressable */
	rom_start = map_rom(ROM_START, ROM_END - ROM_START);
	rom_end = rom_start + ROM_END - ROM_START;

	//printf("Rom Remapped form %x to %x\n", ROM_START, rom_start);

	if(rom_start == 0)
	{
		printf("Cannot Map ROM\n");
		return 1;
	}

	/* First check for the existance of BIOS32 service. */
	hptr = check_bios32();
	if(!hptr)
	{
		printf("Your System Does not Support BIOS32 service.\n");
		return 1;
	}

	memcpy(&bios32_header, hptr, sizeof(struct bios32));
	printf("BIOS32 Found at %X\n", bios32_header.entry);

	if(bios32_header.entry >= 0xE0000000)
	{
		printf("Entry is out of Address Range. %X\n", bios32_header.entry);
		return 1;
	}

	/* We need to change the Physical Address to Virtual address */
	if(bios32_header.entry >= ROM_START && bios32_header.entry <= ROM_END)
	{
		offset = bios32_header.entry - ROM_START;
		bios32_header.entry = rom_start + offset;
	}

	/* Now Check for the presence of PCI Bios */
	ret = BIOS32_exist(PCI_ID, &pci, &bios32_header);
	if(ret != SUCCESSFUL)
	{
		printf("PCI BIOS Does not exist. error %X.\n", ret);
		return 1;
	}

	//printf("PCI BIOS Found at %X\n", pci.base);
	if(pci.base >= 0xE0000000)
	{
		printf("PCI BIOS Entry is out of Address Range. %X\n", pci.base);
		return 1;
	}

	/* Change Physical Address to Virtual */
	if((pci.base >= ROM_START) && (pci.base <= ROM_END))
	{
		offset = pci.base - ROM_START;
		pci.address = rom_start + offset + pci.offset;
	}

	ret = PCIBIOS_ispresent(&pci);
	if(ret != 0)
		return 1;

	search_pci_devices(&pci);
	return 0;
}
