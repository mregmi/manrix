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
 * It handles bit field operations.
 */

#ifndef __ASM_BIT_H
#define __ASM_BIT_H

/* Set a bit in the bitmap */
#define set_bit(bitmap, index)	\
	{			\
	asm volatile (		\
		"bts %1, %0"	\
    	:"=m"(*bitmap) 	\
        :"Ir"(index)	\
		);		\
	}

/* Clear a bit in the bitmap */
#define clear_bit(bitmap, index)	\
	{				\
		asm volatile(			\
		"btr %1, %0"		\
        :"=m"(*bitmap)		\
        :"Ir"(index)		\
		);			\
	}

/* Check if the given bit is set or not. */
static inline int check_set(volatile unsigned int bitmap, int index)
{
	register int result;
	asm volatile(
		"btl %1, %2\t\n"
		"sbbl %0, %0\t\n"
		:"=r"(result)
		:"r"(index), "r"(bitmap)
		);
	return result;
}

/*
 * Find the first bit set and return its pos
 * undefined if none are set
 */
static __inline__ int ffs(volatile unsigned long x)
{
	register int r;
	
	if(!x)
		return 0;
	
	__asm__ volatile("bsfl %1,%0\n\t"
		: "=r" (r)
		: "g" (x)
		);
	return r;
}

/*
 * Find the first cleared bit and return its pos
 * undefined if none are clear
 */
static __inline__ int ffc(volatile unsigned long x)
{
	register int r;

	if(!x)
		return 0;
	
	__asm__ volatile("bsfl %1,%0\n\t"
		:"=r" (r)
		: "g" (~x)
		);
	return r;
}

#endif /* __ASM_BIT_H */
