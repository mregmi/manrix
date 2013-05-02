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
 * File: mman.c
 *  memory management implementation
 */

#include <sys/mman.h>
#include <mrx/pager.h>

#include <asm/vm.h>
#include <asm/ipc.h>
#include <asm/syscall.h>

/*
 * mlock - lock a range of process address space (REALTIME)
 */
int mlock(const void *addr, size_t len)
{
	return syscall_vm_lock((void *)addr, len, 0);
}

/*
 * mlockall - lock/unlock the address space of a process
 */
int mlockall(int flags)
{
	return syscall_vm_lock((void *)NULL, 0, flags);
}


/*
 * munlock - unlock a range of process address space (REALTIME)
 */
int munlock(const void *addr, size_t len)
{
	return syscall_vm_unlock(addr, len, 0);
}

/*
 * munlockall - lock/unlock the address space of a process
 */
int munlockall(void)
{
	return syscall_vm_unlock(NULL, 0, 0);
}

/*
 * mprotect - set protection of memory mapping
 */
int mprotect(void *addr, size_t len, int prot)
{
	return syscall_vm_protect(addr, len, prot);
}

/*
 * mmap - map pages of memory
 */
void *mmap(void *addr, size_t len, int prot, int flags,
       int fildes, off_t off)
{
	struct vn_object vn;

	if(fildes == -1)
		flags |= VM_MAP_ANON;
	else
		flags |= VM_MAP_FILE;

	vn.fd = fildes;
	vn.va = (unsigned long)addr;
	vn.offset = off;
	vn.size = len;
	vn.map_type = flags;
	vn.protection = prot;
	vn.pager_id = ID_FSIO_MNGR;

	return ((void *)syscall_vm_map(&vn, flags));

//	return (void *)vn.va;
}

/*
 * munmap - unmap pages of memory
 */
int munmap(void *addr, size_t len)
{
	return syscall_vm_deallocate(addr, len);
}

/*
 * msync - synchronize memory with physical storage
 */
int msync(void *addr, size_t len, int flags)
{
	/*
	 * Yes, success. because we havent implemented swapping yet.
	 * All memory is synchronised.
	 */
	return 0;
}

/*
 * Increase the data Segment area by incr bytes
 */
void *sbrk(int incr)
{
	static void *prev = NULL;
	unsigned long addr = 0;

	if(incr <= 0)
		return prev;

	

	syscall_vm_allocate(&addr, incr);

	prev = (void *)addr;
	return prev;
}
