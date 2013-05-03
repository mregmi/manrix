/**
 * ManRiX Microkernel
 * Copyright (C) 2004, 2005
 * 	Manish Regmi (regmi dot manish at gmail dot com)
 *
 * This Program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This Program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 **/
/**
 * File:- apic.h
 * 	The Intel APIC (Advanced Programmable Interrupt Controller) handling declarations.
 **/


#ifndef __ASM_APIC_H
#define __ASM_APIC_H

#ifdef MANRIX_APIC

#ifndef __MANRIX_TYPES_H
 #include <kernel/types.h>
#endif

/* I/O APIC */
#define IOAPIC_BASE		0xFEC00000	/* I/O APIC base address */
#define IOREGSEL		IOAPIC_BASE	/* I/O APIC register selector */
#define IOWIN			0xFEC00010	/* I/O APIC window register */

#define IOAPICID		0x00		/* I/O APIC ID register:4bit(24-27) is ID */
#define IOAPICVER		0x01		/* I/O APIC Version Register: 0-7=ver, 16-23=max redirection entry */
#define IOAPICARB		0x02		/* I/O APIC Arbritation Register: 24-27 IOAPIC arbritation */

/**
 * I/O redirection table structure.
 * There are 24 such structures in each I/O APIC chip.
 * 0 - 15 are used for IRQ.
 * 16 - 19 are for PCI IRQ steering.
 * 20 and 21 are for NMI and INIT or general purpose.
 * 22 is for general purpose use.
 * 23 is for SMI interrupt
 **/
struct ioapic_redir_table {
	unsigned char vector;		/* Interrupt Vector used for this entry */
	unsigned delivery_mode:3,	/* Delivery Mode (see manual) */
			 dest_mode:1,		/* Destination mode physical(0) or logical(1) */
			 delivery_status:1,	/* delivery status */
			 polarity:1,		/* polarity of the interrupt
			 			 * signal. 0=High active, 1=Low active.
			 			 */
			 remote_irr:1,		/* ??? */
			 trigger_mode:1,	/* Edge or level triggered */
			 int_mask:1,		/* Interrupt mask bit */
			 reserved1:7;		/* reserved */
	unsigned long reserved2;
	unsigned char destination;	/* Destination CPU */
} __attribute__ ((packed));

/**
 * Reads the 32 bit words from 'which' I/O Apic chip,
 * from an offset 'offset'.
 * Make sure the address is mapped.
 * FIXME: For Now we Only support one I/O APIC Chip.
 **/
static inline unsigned long read_ioapic_reg(int offset, int which)
{
	register unsigned long data;

	*((volatile unsigned long *)IOREGSEL) = offset;
	data = *((volatile unsigned long *)IOWIN);

	return data;
}


/**
 * Writes the 32 bit words to 'which' I/O Apic chip,
 * at an offset 'offset'.
 * Make sure the address is mapped.
 * FIXME: For Now we Only support one I/O APIC Chip.
 **/
static inline void write_ioapic_reg(int offset, int which, unsigned long data)
{
	*((volatile unsigned long *)IOREGSEL) = offset;
	*((volatile unsigned long *)IOWIN) = data;
}


/*
 * Read the I/O APIC table entry.
 */
static inline int read_ioapic_table(int apic, int irq, struct ioapic_redir_table *table)
{
	u32 *ptr;
	u32 data;

	ptr = (u32 *)table;
	data = read_ioapic_reg(irq + 0x10, apic);
	*ptr++ = data;
	data = read_ioapic_reg(irq + 0x11, apic);
	*ptr = data;

	return 0;
}

/*
 * Write the I/O APIC table entry.
 */
static inline int write_ioapic_table(int apic, int irq, struct ioapic_redir_table *table)
{
	u32 *ptr;
	u32 data;

	ptr = (u32 *)table;
	data = *ptr++;
	write_ioapic_reg(irq + 0x10, apic, data);
	data = *ptr;
	write_ioapic_reg(irq + 0x11, apic, data);

	return 0;
}


/* Local APIC registers Locations (Memory Mapped) */
#define LAPIC_ID			0x20	/* Local APIC ID register */
#define LAPIC_VERSION		0x30	/* Local APIC Version Registor */
#define LAPIC_TASK_PRIO		0x80	/* Local APIC Task Priority Register */
#define LAPIC_ARB_PRIO		0x90	/* Local APIC Arbritation Priority Register */
#define LAPIC_PROC_PRIO		0xA0	/* Local APIC Processor Priority Register */
#define LAPIC_EOI			0xB0	/* Local APIC EOI Register */
#define LAPIC_SIVR			0xF0	/* Local APIC Spurious Interrupt vector register */
#define LAPIC_LOG_DEST		0xD0	/* Local APIC Logical Destination Register */
#define LAPIC_CMD_LOW		0x300	/* Local APIC Commad register lower bits */
#define LAPIC_CMD_HIGH		0x310	/* Local APIC Commad register higher bits */

/* Read and Write the Local APIC registers */
#define LAPIC_READ(index)		(*((volatile unsigned long *)(LAPIC_BASE + index)))
#define LAPIC_WRITE(index, val)		(*((volatile unsigned long *)(LAPIC_BASE + index)) = (unsigned long)val)

/*
 * Local Vector Table (LVT) structure
 */
struct lvt{
	unsigned vector:8,			/* Interrupt Vector used for this entry */
			 delivery_mode:3,	/* Delivery Mode (see manual) */
			 reserved1:1,		/* reserved1 */
			 delivery_status:1,	/* delivery status */
			 polarity:1,		/* polarity of the interrupt
			 					 * signal. 0=High active, 1=Low active.
			 					 */
			 remote_irr:1,		/* ??? */
			 trigger_mode:1,	/* Edge or level triggered */
			 int_mask:1,		/* Interrupt mask bit */
			 timer_mode:1,		/* Timer Mode 0: One-shot, 1: Periodic */
			 reserved1:14;		/* reserved */
} __attribute__ ((packed));

/*
 * Interrupt Command Register.
 */
struct icr{
	unsigned char vector;		/* Interrupt Vector used for this entry */
	unsigned delivery_mode:3,	/* Delivery Mode (see manual) */
			 dest_mode:1,		/* Destination mode physical(0) or logical(1) */
			 delivery_status:1,	/* delivery status */
			 reserved1:1,		/* Reserved */
			 level:1,			/* Level: 0 = De-assert, 1 = Assert */
			 trigger_mode:1,	/* Edge or level triggered */
			 reserved2:2,		/* reserved */
			 dest:2;			/* Destination Shorthand (see manual) */
	unsigned long reserved2;
	unsigned char destination;	/* Destination CPU */
} __attribute__ ((packed));

extern int lapic;

#endif /* MANRIX_APIC */
#endif	/* __ASM_APIC_H */
