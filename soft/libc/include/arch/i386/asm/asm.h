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
 * File: asm.h
 * This header contains macros which contain inline assembly.
 *
 */

#ifndef __ASM_ASM_H
#define __ASM_ASM_H

#include <string.h>

/* Send a byte to the port */
static inline void outportb(unsigned char value, unsigned short port)
{
	__asm__ volatile(
		"outb %%al, %%dx"
		:
		:"a" (value),"d" (port)
		);
}


/* Get a byte sized data from the port */
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

/* Send a word to the port */
#define outportw(value, port)	\
__asm__ volatile(						\
		"outw %%ax, %%dx"		\
		:						\
		:"a" (value),"d" (port)	\
		)


/* Get a word sized data from the port */
static inline unsigned short inportw(unsigned short port)
{
	register unsigned short ret;
	__asm__ volatile
			(
			"inw %%dx, %%ax"
			:"=a" (ret)
			:"d" (port)
			);
	return ret;
}

/* Send a double word to the port */
#define outportl(value, port)	\
__asm__ volatile(						\
		"outl %%eax, %%dx"		\
		:						\
		:"a" (value),"d" (port)	\
		)


/* Get a double word sized data from the port */
static inline unsigned long inportl(unsigned short port)
{
	register unsigned long ret;
	__asm__ volatile
			(
			"inl %%dx, %%eax"
			:"=a" (ret)
			:"d" (port)
			);
	return ret;
}

/* Send a string of bytes to the port */
#define outsb(data, length, port)	\
__asm__ volatile(						\
		"rep outsb \n\t"		\
		:						\
		:"S" (data), "c"(length), "d" (port)	\
		)


/* Get a string of byte sized data from the port */
#define insb(data, length, port)	\
{									\
	__asm__ volatile				\
			(						\
			"cld\n\t"				\
			"rep insb \n\t"			\
			:						\
			: "d"(port), "D"(data), "c"(length)	\
			:"edi"			\
			);						\
}

/* Send a string of words to the port */
static inline void outsw(void *data, int length, unsigned short port)
{
__asm__ volatile(
		"rep outsw\n\t"
		:
		:"d" (port),"S"(data), "c"(length/2)
		);
}


/* Get a string of word sized data from the port */
static inline void insw(void *data, int length, unsigned short port)
{
	int d0, d1, d2;
	__asm__ volatile(
			"cld\n\t"
			"rep insw \n\t"
			:"=&d"(d0), "=&D"(d1), "=&c"(d2)
			:"0"(port), "1"(data), "2"(length/2)
			);
}

/* Send a string of dword to the port */
static inline void outsl(void *data, int length, unsigned short port)
{
	__asm__ volatile(
		"rep outsl \n\t"
		:
		:"S" (data), "c"(length/4), "d" (port)
		);
}

/* Get a string of double word sized data from the port */
#define insl(data, length, port)	\
{									\
	__asm__ volatile				\
			(						\
			"rep insl \n\t"			\
			:						\
			:"D" (data), "c"(length/4), "d"(port)	\
			);						\
}

static inline void maintain_curstack(char *ptr, int size)
{
	char *curstack;
	
	asm volatile(
		    "movl %%esp, %0\n\t"
		:"=r"(curstack)	
		    );
//	curstack = ((unsigned long)curstack & ~(size-1));
	memcpy(ptr, curstack, size);
}
	
#endif	/* __ASM_ASM_H */
