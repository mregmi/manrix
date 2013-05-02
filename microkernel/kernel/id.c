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
 * File: id.c
 * This file implements the allocation of ids like pid, tid.
 * The bitmap is maintained for each id type. The ids which require
 * uniqueness with each other use the same bitmap. The set bit in the
 * bitmap identifies the allocated id. Its index gives the actual value.
 * The first few values are RESERVED and are not allocated for user programs.
 * We keep a hint that point in the bitmap which was freed or the last index
 * that gave id.
 */

#include <string.h>

#include <kernel/scheduling.h>
#include <kernel/vm.h>

#include <asm/asm.h>	/* for ffc() */
//#define BITMAP_MASK

/* A bitmap for Pid */
static unsigned int *pidbitmap;

/* A bitmap for tid */
static unsigned int *tidbitmap;

/* A bitmap for timerid */
static unsigned int *timeridbitmap;

/* the hints for fast lookup. */
static int pidhint = 0;
static int tidhint = 0;
static int timeridhint = 0;


void id_init()
{
	pidbitmap = (unsigned int *)kmem_alloc(4096);
	tidbitmap = (unsigned int *)kmem_alloc(4096);
	timeridbitmap = (unsigned int *)kmem_alloc(4096);
	memset(timeridbitmap, 0, 4096);
	memset(pidbitmap, 0, 4096);
	memset(tidbitmap, 0, 4096);
}


static inline int alloc_id(unsigned int *bitmap, int *hintptr)
{
	register int index;
	register int hint = *hintptr;

	if(bitmap[hint] == MAX_INT_VAL)
	{
		hint = 0;
		while(bitmap[hint] == MAX_INT_VAL)
			hint++;
		if(hint > (4096/BITS_PER_INT))
			return -1;
	}

	index = ffc(bitmap[hint]);
	set_bit(&bitmap[hint], index);
	index += hint * BITS_PER_INT;
	*hintptr = hint;

    return index;
}

static inline void dealloc_id(int id, unsigned int *bitmap, int *hintptr)
{
	register int hint = *hintptr, index;

	hint = id / BITS_PER_INT;
	index = id % BITS_PER_INT;

	clear_bit(&bitmap[hint], index);
	*hintptr = hint;
}

/*
 * Allocate a unique pid to a calling process.
 */
int alloc_pid()
{
	return (FIRST_USER_PID + alloc_id(pidbitmap, &pidhint));
}

/*
 * Allocate a unique tid to a calling thread.
 */
int alloc_tid()
{
	return (FIRST_USER_TID + alloc_id(tidbitmap, &tidhint));
}

/*
 * deallocate the given pid.
 */
void dealloc_pid(pid_t pid)
{
	dealloc_id(pid - FIRST_USER_PID, pidbitmap, &pidhint);
}

/*
 * deallocate the given tid.
 */
void dealloc_tid(tid_t tid)
{
	dealloc_id(tid - FIRST_USER_TID, tidbitmap, &tidhint);
}

/*
 * Allocate a unique timerid.
 */
int alloc_timerid()
{
	return (1 + alloc_id(timeridbitmap, &timeridhint));
}

/*
 * deallocate the given pid.
 */
void dealloc_timerid(int timer_id)
{
	dealloc_id(timer_id - 1, timeridbitmap, &timeridhint);
}

