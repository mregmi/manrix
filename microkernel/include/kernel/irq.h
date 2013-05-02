/*
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
 */
/*
 * File: irq.c
 *
 * This file handles interrupt requests. We only redirect the IRQ to the user
 * level threads.
 */

#ifndef __IRQ_H
#define __IRQ_H

#include <string.h>

#include <asm/asm.h>
#include <kernel/list.h>
#include <kernel/scheduling.h>
#include <kernel/vm.h>

/*
 * Total no of IRQ lines.
 * TODO: put it in Arch dependent part.
 */
#define MAX_IRQ			16

/*
 * The irq structure.
 * There is one Structure For Each IRQ line.
 * Since the IRQ lines can be Shared, there can be more than
 * one Handlers for the Same Irq. In that case the the Handlers
 * are put in Linked List.
 */
struct irq{
	int id;
	int flag;
	struct linked_list list;
	struct irq *next;
	void (*handler)(void);
	thread_t owner;
	spinlock_t lock;
};
typedef struct irq *irq_t;

typedef struct {
	struct irq *irq;
} *irqhashtable_t;


extern irq_t find_by_irqid(int );
extern void IRQ_init();
extern void IRQ_arch_init();

extern void handle_IRQ(int );
extern void handle_IRQ_event(thread_t , struct stack_regs *);


#endif /* __IRQ_H */
