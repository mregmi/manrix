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
 * File: vm.h
 */

#ifndef __ASM_VM_H
#define __ASM_VM_H

#include <errno.h>
#include <asm/syscalldefs.h>

/* vm */
#define VM_MAP_ZERO_FILL	1 /* Entry is zero fill */

/* The object in the entry can be mapped anywhere. */
#define VM_MAP_OBJECT_ANYWHERE	2

#define VM_MAP_OBJECT_FIXED	4 /* The address should be fixed */

/* The object in the map has anonymous Mapping */
#define VM_MAP_ANON		8
#define VM_MAP_LOWMEM		0x40
#define VM_MAP_FILE		0x80	/* Memory mapped file */

extern inline int syscall_vm_allocate(unsigned long *ptr, size_t size)
{
	int ret;

	asm volatile (
		"int $0x30\n\t"
		:"=a"(ret), "=b"(*ptr)
		:"0"(SYS_VM_ALLOCATE),"1"(*ptr), "c"(size)
		);

	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	return ret;
}

extern inline int syscall_vm_deallocate(void *ptr, size_t size)
{
	register int ret;

	asm volatile (
		"int $0x30\n\t"
		:"=a"(ret)
		:"0"(SYS_VM_DEALLOCATE),"b"(ptr), "c"(size)
		);

	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	return ret;
}

extern inline int syscall_vm_lock(void *ptr, size_t size, int flag)
{
	register int ret;

	asm volatile (
		"int $0x30\n\t"
		:"=a"(ret)
		:"0"(SYS_VM_LOCK), "b"(ptr), "c"(size), "d"(flag)
		);

	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	return ret;
}

extern inline int syscall_vm_unlock(const void *ptr, size_t size, int flag)
{
	register int ret;

	asm volatile (
		"int $0x30\n\t"
		:"=a"(ret)
		:"0"(SYS_VM_UNLOCK), "b"(ptr), "c"(size), "d"(flag)
		);

	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	return ret;
}

extern inline int syscall_vm_map(struct vn_object *vn, int flag)
{
	register int ret;

	asm volatile(
		"int $0x30\n\t"
		:"=a"(ret)
		:"0"(SYS_VM_MAP), "b"(vn), "c"(flag)
		);

	if(ret < 0)
	{
		errno = -ret;
		return 0;
	}

	return ret;
}

extern inline int syscall_vm_protect(void *ptr, size_t size, int flag)
{
	register int ret;

	asm volatile(
		"int $0x30\n\t"
		:"=a"(ret)
		:"0"(SYS_VM_PROTECT), "b"(ptr), "c"(size), "d"(flag)
		);

	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	return ret;
}

#endif /* __ASM_VM_H */
