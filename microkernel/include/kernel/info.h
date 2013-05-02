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
 * File: info.h
 * 		Gives the system information.
 */

#ifndef __KERNEL_INFO_H
#define __KERNEL_INFO_H

#include <string.h>
#include <kernel/list.h>
#include <kernel/scheduling.h>
#include <kernel/atomic.h>

/* System info */
struct sysinfo{
	atomic_t total_process;
	atomic_t total_threads;

	vm_size_t total_mem;
	vm_size_t used_mem;

	vm_size_t total_ram;
	vm_size_t used_ram;

	/* List of all processes */
	struct linked_list proclist;
};

/* Process info */
struct procinfo {
};

//extern struct sysinfo info;

#define UN_DATA 	(sizeof(struct linked_list))

extern struct sysinfo sysinfo;
extern spinlock_t syslock;

/*
 * Add the Process to the list
 */
static inline void sysinfo_add_process(process_t p)
{
	atomic_inc(&sysinfo.total_process);
	spin_lock(&syslock);
	list_add(&sysinfo.proclist, p, proclist, process_t);
	spin_unlock(&syslock);
}

/*
 * Remove the Process to the list
 */
static inline void sysinfo_del_process(process_t p)
{
	atomic_dec(&sysinfo.total_process);
	spin_lock(&syslock);
	list_del(&sysinfo.proclist, p, proclist, process_t);
	spin_unlock(&syslock);
}

#endif /* __KERNEL_INFO_H */
