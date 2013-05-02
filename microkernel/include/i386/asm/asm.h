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
 * File: asm.h
 * 	The the Functions implented as inline Assembly.
 */

#ifndef __ASM_ASM_H
#define __ASM_ASM_H

#include <asm/x86.h>
/*
 * Send a value to the port
 */
#define outportb(value,port) \
__asm__ volatile("outb %%al,%%dx"::"a" (value),"d" (port))

/*
 * Clear interrupt
 */
#define CLI()		asm("cli\n\t")

/*
 * Set the interrupt Flag
 */
#define STI()		asm("sti\n\t")

/*
 * Set a bit in the bitmap
 */
#define set_bit(bitmap, index)	\
	{			\
	asm volatile (		\
		"btsl %1, %0"	\
    	:"=m"(*bitmap) 	\
        :"Ir"(index)	\
		);		\
	}

/*
 * Clear a bit in the bitmap
 */
#define clear_bit(bitmap, index)	\
	{				\
		asm volatile(			\
		"btrl %1, %0"		\
        :"=m"(*bitmap)		\
        :"Ir"(index)		\
		);			\
	}

/*
 * Check if the given bit is set or not.
 */
static inline int check_set(volatile unsigned int bitmap, int index)
{
	int result;
	asm volatile(
		"btl %1, %2\t\n"
		"sbbl %0, %0\t\n"
		:"=r"(result)
		:"r"(index), "r"(bitmap)
		);
	return result;
}

/*
 * Get a byte sized data from the port
 */
static inline unsigned char inportb(unsigned short port)
{
	register unsigned char ret;
	__asm__ volatile
			(
			"inb %%dx,%%al"
			:"=a" (ret)
			:"d" (port)
			);
	return ret;
}

/*
 * Send a word to the port
 */
#define outportw(value, port)	\
__asm__ (						\
		"outb %%ax, %%dx"		\
		:						\
		:"a" (value),"d" (port)	\
		)


/*
 * Get a word sized data from the port
 */
static inline unsigned short inportw(unsigned short port)
{
	register unsigned short ret;
	__asm__ volatile
			(
			"inb %%dx, %%ax"
			:"=a" (ret)
			:"d" (port)
			);
	return ret;
}

/*
 * Send a double word to the port
 */
#define outportl(value, port)	\
__asm__ (						\
		"outb %%eax, %%dx"		\
		:						\
		:"a" (value),"d" (port)	\
		)


/*
 * Get a double word sized data from the port
 */
static inline unsigned long inportl(unsigned short port)
{
	register unsigned long ret;
	__asm__ volatile
			(
			"inb %%dx, %%eax"
			:"=a" (ret)
			:"d" (port)
			);
	return ret;
}

/*
 * Send a string of bytes to the port
 */
#define outsb(data, length, port)	\
__asm__ (						\
		"rep outsb \n\t"		\
		:						\
		:"S" (data), "c"(length), "d" (port)	\
		)


/*
 * Get a string of byte sized data from the port
 */
#define insb(data, length, port)	\
{									\
	__asm__ volatile				\
			(						\
			"rep insb \n\t"			\
			:						\
			:"D" (data), "c"(length), "d"(port)	\
			);						\
}

/*
 * Send a string of words to the port
 */
#define outsw(data, length, port)	\
__asm__ (						\
		"rep outsw \n\t"		\
		:						\
		:"S" (data), "c"(length), "d" (port)	\
		)


/*
 * Get a string of word sized data from the port
 */
#define insw(data, length, port)	\
{									\
	__asm__ volatile				\
			(						\
			"rep insw \n\t"			\
			:						\
			:"D" (data), "c"(length), "d"(port)	\
			);						\
}

/*
 * Send a string of dword to the port
 */
#define outsl(data, length, port)	\
__asm__ (						\
		"rep outsl \n\t"		\
		:						\
		:"S" (data), "c"(length), "d" (port)	\
		)


/*
 * Get a string of double word sized data from the port
 */
#define insl(data, length, port)	\
{									\
	__asm__ volatile				\
			(						\
			"rep insl \n\t"			\
			:						\
			:"D" (data), "c"(length), "d"(port)	\
			);						\
}


/*
 * Read fs register
 */
static inline unsigned short read_fs()
{
	register unsigned short fs;
	__asm__ volatile(
		"movw %%fs, %0"
		:"=r"(fs)
		);
	return fs;
}

/*
 * Read GS register
 */
static inline unsigned short read_gs()
{
	register unsigned short gs;
	__asm__ volatile(
		"movw %%gs, %0"
		:"=r"(gs)
		);
	return gs;
}

/*
 * Write to fs register
 */
static inline void write_fs(unsigned short fs)
{
	__asm__ volatile(
		"movw %0, %%fs"
		:
		:"r"(fs)
		);
}

/*
 * Write to gs register
 */
static inline void write_gs(unsigned short gs)
{
	__asm__ volatile(
		"movw %0, %%gs"
		:
		:"r"(gs)
		);
}

/*
 * Find the first bit set and return its pos
 * undefined if none are set
 */
static __inline__ int ffs(volatile unsigned int x)
{
	register int r;

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
static __inline__ int ffc(volatile unsigned int x)
{
	register int r;

	__asm__ volatile(
		"bsfl %1,%0\n\t"
		:"=r" (r)
		: "r" (~x)
		);
	return r;
}

/*
 * Find the last bit set and return its pos
 * undefined if none are set
 */
static __inline__ int frs(volatile unsigned int x)
{
	register int r;
	if(!x)
		return -1;
	
	__asm__ volatile(
		"bsrl %1, %0\n\t"
		: "=r" (r)
		: "r" (x)
		);
	return r;
}

#endif	/* __ASM_ASM_H */
