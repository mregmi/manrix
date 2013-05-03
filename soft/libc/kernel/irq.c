/*
 * Libc for ManRiX OS
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
 */

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <asm/syscall.h>

typedef void (* ihand_t)();

struct irq_data{
	int irq;
	ihand_t hand;
};

int IRQ_common(struct irq_data *data)
{
	ihand_t handle;

	syscall_change_iopl(3);
	syscall_IRQ_request(data->irq);
	handle = data->hand;
	free(data);

	while(1)
	{
		syscall_IRQ_wait();
		handle();
	}
	pthread_exit(NULL);
}

int IRQ_request(int irq, ihand_t hand)
{
	struct irq_data *idata;
	pthread_attr_t attr;
	struct sched_param parm;

	pthread_attr_init(&attr);
	parm.sched_priority = PRIO_RT_MAX;
	pthread_attr_setschedparam(&attr, &parm);
	idata = malloc(sizeof(struct irq_data));
	idata->irq = irq;
	idata->hand = hand;
	pthread_create(NULL, &attr, IRQ_common, idata);
}
