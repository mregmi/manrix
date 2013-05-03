/*
 * ManRiX Microkernel
 * Copyright (C) 2004, 2005	Manish Regmi (regmi dot manish at gmail dot com)
 *				Rajesh R.C (rajee5283 at hotmail dot com)
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
 * File: info.c
 * 		Gives the system information.
 */

#include <string.h>
#include <kernel/info.h>

struct sysinfo sysinfo;
spinlock_t syslock = SPIN_LOCK_UNLOCKED;

int syscall_sysinfo(void *ptr, struct sysinfo *sysinf)
{
	if(sysinf)
		ker_to_usr((void *)sysinf, (void *)&sysinfo, sizeof(struct sysinfo) - KERNEL_DATA);

	return 0;
}

/* Get the array of pids */
int syscall_getpidlist(void *ptr, void *data)
{
	pid_t *pidlist;
	process_t proc;
	int index = 0;
	size_t size;

	size = atomic_get(&sysinfo.total_process) * sizeof(pid_t);
	pidlist = (pid_t *)kmem_alloc(size);

	for(proc = (process_t)sysinfo.proclist.next; proc != (process_t)&sysinfo.proclist ;
		proc = (process_t)proc->proclist.next)
	{
		pidlist[index] = proc->pid;
		index++;
	}

	ker_to_usr((void *)data, (void *)pidlist, size);

	kmem_free((vm_offset_t)pidlist, size);
	return 0;
}

int syscall_getpstat()
{
	return 0;
}

int syscall_gettstat()
{
	return 0;
}
