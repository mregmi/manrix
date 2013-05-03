/**
 * ManRiX Microkernel
 * Copyright (C) 2004, 2005
 * 	Manish Regmi (regmi dot manish at gmail dot com) and
 * 	Rajesh R.C   (rajesh dot rc at gmail dot com)
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
 * File: pic.c
 *
 * This file handles chips like programmable interrupt controller pic(8259a, I/O APIC).
 * Only basic features are handled here. The devices are handled in user space.
 **/
/*
 * I/O APIC chip support by Manish Regmi.
 */

#include <kernel/scheduling.h>
#include <string.h> 
#include <asm/8259a.h>
#include <asm/apic.h>
#include <asm/intelmp.h>
#include <asm/asm.h>

static int cpunum = 0;
static int ioapic = 0;
extern struct mp_processor processors[MAX_CPU];
extern int N_PROCS;

void init_8259a(int pic1_start, int pic2_start)
{
	outportb(ICW1_ICW1|ICW1_ICW4, PIC1_COMMAND);
	outportb(ICW1_ICW1|ICW1_ICW4, PIC2_COMMAND);

	outportb(pic1_start, PIC1_DATA);
	outportb(pic2_start, PIC2_DATA);

	outportb(4, PIC1_DATA);
	outportb(2, PIC2_DATA);

	outportb(ICW4_8086, PIC1_DATA);
	outportb(ICW4_8086, PIC2_DATA);

	/*
	 * Disable all interrupts for now.
	 * They will be enabled when their respective
	 * handlers are initialized.
	 */

	outportb(0xFF, PIC1_DATA);
	outportb(0xFF, PIC2_DATA);
}

static void disable_8259a()
{
	outportb(0xFF, PIC1_DATA);
	outportb(0xFF, PIC2_DATA);
}

/*
 *
 */
void enable_irq(int irq)
{
	if(irq < 8)
		outportb(inportb(PIC1_DATA) & ~(1 << irq), PIC1_DATA);
	else
		outportb(inportb(PIC2_DATA) & ~(1 << (irq-8)) ,PIC2_DATA);
}


void disable_irq(int irq)
{
	if(irq < 8)
		outportb(inportb(PIC1_DATA) | (1 << irq), PIC1_DATA);
	else
		outportb(inportb(PIC2_DATA) | (1 << (irq-8)), PIC2_DATA);
}

#ifdef MANRIX_IOAPIC

/*
 * Get the cpu number.
 * Used for distributing interrupts.
 */
static inline int get_irq_cpu()
{
	int cpu;

	if(cpunum > N_PROCS)
		cpunum = 0;
	cpu = processors[cpunum].lapic_id;
	cpunum++;

	return cpu;
}

/*
 * set the I/O apic vector.
 */
static inline int set_ioapic_intr(int vector, int irq, u8 cpu)
{
	struct ioapic_redir_table table;

	if((vector < 0x10) && (vector > 0xFF))
		return -1;

	if((irq < 0) && (irq > 24))
		return -1;

	memset(&table, 0, sizeof(struct ioapic_redir_table));
	table.vector = vector;
	table.destination = cpu;
	table.delivery_mode = 1;	/* or 0 */
	table.dest_mode = 0;		/* Physical */
	table.polarity = 0;		/* ??? */
	table.trigger_mode = 0;	/* Edge Triggered but can be any */
	table.int_mask = 0;		/* not masked */

	write_ioapic_table(0, irq, &table);
}

/*
 * Initialize the I/O APIC chip.
 * FIXME: How to detect the presence of IOAPIC.
 */
int ioapic_init(int start_desc)
{
	int irq;

	if(!lapic)
	{
		kprintf("I/O APIC Failed. Using 8259A chip. \n");
		return -1;
	}

	/* Write the I/O apic redirection table for 15 hardware interrupts */
	for(irq = 0; irq < 16; irq++)
	{
		set_ioapic_intr(start_desc, irq, get_irq_cpu());
		start_desc++;
	}

	/* Now we are able to use I/O APIC, so disable 8259a */
	disable_8259a();
	ioapic = 1;

	return 0;
}

/*
 * Disable I/O APIC IRQ
 */
int disable_ioapic_irq(int irq)
{
	struct ioapic_redir_table table;

	read_ioapic_table(0, irq, &table);
	table.int_mask = 1;
	write_ioapic_table(0, irq, &table);

	return 0;
}

/*
 * Enable I/O APIC IRQ
 */
int enable_ioapic_irq(int irq)
{
	struct ioapic_redir_table table;

	read_ioapic_table(0, irq, &table);
	table.int_mask = 0;
	write_ioapic_table(0, irq, &table);

	return 0;

}

#endif /* MANRIX_IOAPIC */

/**
 * Initialize the PIC chip.
 * first parameter is the starting interrupt num for pic1.
 * second one is for pic2, which is used by 8259a's only.
 **/
void pic_init(int pic1_start, int pic2_start)
{

#ifdef MANRIX_LAPIC
	apic_init();
#endif

#ifdef MANRIX_IOAPIC
	if(ioapic_init(pic1_start) == 0)
		return ;
#endif

	init_8259a(pic1_start, pic2_start);
}

