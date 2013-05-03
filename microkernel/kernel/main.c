/**
 * ManRiX Microkernel
 * Copyright (C) 2004, 2005
 * 	Manish Regmi (regmi dot manish at gmail dot com) and
 * 	Rajesh R.C   (rajee5283@hotmail.com)
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
 * File:- main.c
 * 	This is the main file of ManRiX OS. The Kernel Initialization is done
 * 	here.
 **/

#include <kernel/types.h>
#include <kernel/message.h>
#include <kernel/scheduling.h>
#include <kernel/kernel_stdio.h>
#include <kernel/irq.h>
#include <kernel/cpu.h>

#include <asm/asm.h>
#include <asm/multiboot.h>

extern thread_t init_thread;
extern int init;

/*
 * Idle threads, for each CPU. They should never
 * sleep or return.
 */
void idle_thread()
{
	int index = 0;

	while(1)
	{
		index++;
	}
}


/*
 * This is the C initialization Routine called from
 * assembly kernel_entry.S.
 */
void ManRiX_main()
{
	struct cpu *cpu;
	
	/*
	 * Architecture dependent initialization.
	 */
	arch_setup();
	
	/*
	 * Initialize the Virtual Memory SubSystem (VM)
	 */
	vm_init();
	
	/*
	 * Initialize the process SubSystem.
	 */
	process_init();
	
	/*
	 * Initialize the Scheduler SubSystem.
	 */
	sched_init();

	/*
	 * Initialize IRQ.
	 */
	IRQ_init();

	/*
	 * Initialize time and timer managemnt code.
	 */
	time_init();

	/*
	 * Initialize the Modules required to bring the
	 * System Up. Those Modules are Loaded by the Bootloader.
	 */
	modules_init();

	init = 1;

//	remove_thread(init_thread);
	init_thread->priority_stat = PRIO_SYS_MIN; /* for making schedular not to reenqueue this thread */
	cpu = get_cpu();
//	kprintf("%X cpus %X", cpu, &cpus);
	cpu->idle = init_thread;
	STI();
	
	idle_thread();
}


