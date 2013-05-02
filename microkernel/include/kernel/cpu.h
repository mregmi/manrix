/*
 * ManRiX Microkernel
 * Copyright (C) 2004, 2005
 * 	Manish Regmi (regmi dot manish at gmail dot com) and
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
 * File: cpu.h
 * 	CPU information for MultiProcessors.
 */

#ifndef __KERNEL_CPU_H
#define __KERNEL_CPU_H

#include <kernel/scheduling.h>

struct cpu {
	thread_t idle;
};

extern struct cpu cpus[MAX_CPU];

#ifndef MANRIX_SMP
/*
 * Get the CPU where the code is executing
 */
static inline struct cpu *get_cpu()
{
	return ((struct cpu *)&cpus[0]);
}

/*
 * Get the idle thread for the CPU.
 */
static inline thread_t get_idle_thread(struct cpu *cpu)
{
	return init_thread;
}

/*
 * Get the runqueue for currently executing code.
 */
static inline struct rqueue *get_cur_runqueue()
{
	return &runqueue[0];
}

/*
 * Get the runqueue where thr lies.
 */
static inline struct rqueue *get_thr_runqueue(thread_t thr)
{
	return &runqueue[0];
}

#else
/*
 * Get the CPU where the code is executing
 */
static inline struct cpu *get_cpu()
{
	register struct cpu *cpu;

	cpu = &cpus[current->cpu];
	return cpu;
}

/*
 * Get the idle thread for the CPU.
 */
static inline thread_t get_idle_thread(struct cpu *cpu)
{
	return cpu->idle;
}
/*
 * Get the runqueue for currently executing code.
 */
static inline struct rqueue *get_cur_runqueue()
{
	return &runqueue[current->cpu];
}

/*
 * Get the runqueue where thr lies.
 */
static inline struct rqueue *get_thr_runqueue(thread_t thr)
{
	return &runqueue[thr->cpu];
}

#endif /* MANRIX_SMP */

#endif	/* __KERNEL_CPU_H */
