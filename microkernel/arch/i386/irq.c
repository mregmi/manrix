/*
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
 */
/*
 * File: irq.c
 *
 * This file handles interrupt requests. We only redirect the IRQ to the user
 * level threads.
 */

#include <string.h>

#include <asm/asm.h>
#include <kernel/irq.h>
#include <kernel/list.h>
#include <kernel/scheduling.h>
#include <kernel/vm.h>

extern GATE	*idt;

extern void irq_0();
extern void irq_1();
extern void irq_2();
extern void irq_3();
extern void irq_4();
extern void irq_5();
extern void irq_6();
extern void irq_7();
extern void irq_8();
extern void irq_9();
extern void irq_10();
extern void irq_11();
extern void irq_12();
extern void irq_13();
extern void irq_14();
extern void irq_15();


/*
 * Initialize IRQ
 */
void IRQ_arch_init()
{
	set_interrupt_gate(0x20, &irq_0);
	set_interrupt_gate(0x21, &irq_1);
	set_interrupt_gate(0x22, &irq_2);
	set_interrupt_gate(0x23, &irq_3);
	set_interrupt_gate(0x24, &irq_4);
	set_interrupt_gate(0x25, &irq_5);
	set_interrupt_gate(0x26, &irq_6);
	set_interrupt_gate(0x27, &irq_7);
	set_interrupt_gate(0x28, &irq_8);
	set_interrupt_gate(0x29, &irq_9);
	set_interrupt_gate(0x2a, &irq_10);
	set_interrupt_gate(0x2b, &irq_11);
	set_interrupt_gate(0x2c, &irq_12);
	set_interrupt_gate(0x2d, &irq_13);
	set_interrupt_gate(0x2e, &irq_14);
	set_interrupt_gate(0x2f, &irq_15);

}


/*
 * Handle the IRQ event.
 * They are handled in a very Similar Way as
 * Signals.
 */
void handle_IRQ_event(thread_t thr, struct stack_regs *regs)
{
	struct signal_context *frame;

	frame = (struct signal_context *)(regs->user_esp - sizeof(struct signal_context));
	put_user_long((u32)&frame->syscall_return, (unsigned long *)&frame->jmpaddr);
	put_user_word(0xb890, (unsigned short *)&frame->syscall_return[0]); /* nop; movl $0, %%eax */
 	put_user_long(0x0, (unsigned long *)&frame->syscall_return[1]);
	put_user_word(0x30cd, (unsigned short *)&frame->syscall_return[3]);	/* int $0x30 */

	ker_to_usr((void *)&frame->ucontext.uc_mcontext, (void *)regs, sizeof(struct stack_regs));

	regs->user_eip = (u32)thr->context.handler;
	regs->user_esp = (u32)frame;

	thr->context.irqflag |= ON_IRQHAND;
}

