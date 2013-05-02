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
 * File:- string.h
 * 	This header contains a set of architecture specfic macros and functions
 * for handling strings.
 */

#ifndef __ASM_STRING_H
#define __ASM_STRING_H

#ifndef __SYS_TYPES_H
#include <sys/types.h>
#endif

/*
 * Get the Length of the String.
 */
static __inline__ size_t strlen(const char *str)
{
	register size_t __ret;
	__asm__ __volatile__ (
				"cld \n\t"
				"repne \n\t"
				"scasb \n\t"
				"notl %0\n\t"
				"decl %0"
				:"=c" (__ret)
				: "a" (0),"D" (str), "0"(0xffffffff)
				);
	return __ret;
}

/*
 * Concat the two strings.
 */
static __inline__ char *strcat(char *dest, const char *src)
{
	char *__str;
	int d0, d1;
	__asm__ __volatile__ (
			"cld \t\n"
			"movl $0xffffffff, %%ecx\n\t"
			"repne\t\n"
			"scasb\t\n"
			"subl 0xffffffff, %%ecx\t\n"
			"rep\t\n"
			"movsb\t\n"
			:"=&D"(d0), "=&S"(d1)
			:"a"(0),"0"(dest),"1"(src)//, "0"(0xffffffff)
		);
	return __str;
}

/*
 * Copy the String.
 */
static __inline__ char *strcpy(char *dest, const char *src)
{
	char *__str;
	int d0, d1;

	__asm__ __volatile__(
			"cld \n\t"
			"1:\t"
			"lodsb\t\n"
			"stosb\t\n"
			"testb %%al, %%al\n\t"
			"jne 1b\t\n"
			:"=&D"(d0), "=&S"(d1)
			:"0"(dest),"1"(src)
			:"ax"
			);
	return __str;
}

/*
 * Set the Memory Address range to the Given Value.
 */
static __inline__ void memset(void *src, int c, size_t n)
{
	int d0, d1;
	__asm__ __volatile__(
			"cld\t\n"
			"rep\t\n"
			"stosb\t\n"
			:"=&c" (d0), "=&D" (d1)
			:"0"(n),"a"(c),"1"(src)
			:"memory"
			);
}

/*
 * Compare the two Values.
 */
static inline int strcmp(const char * cs,const char * ct)
{
int d0, d1;
register int __res;
__asm__ __volatile__(
	"1:\tlodsb\n\t"
	"scasb\n\t"
	"jne 2f\n\t"
	"testb %%al,%%al\n\t"
	"jne 1b\n\t"
	"xorl %%eax,%%eax\n\t"
	"jmp 3f\n"
	"2:\tsbbl %%eax,%%eax\n\t"
	"orb $1,%%al\n"
	"3:"
	:"=a" (__res), "=&S" (d0), "=&D" (d1)
		     :"1" (cs),"2" (ct));
return __res;
}

/*
static inline void * memcpy(void * dest,const void *src,size_t count)
{
	char *tmp = (char *) dest, *s = (char *) src;

	while (count--)
		*tmp++ = *s++;

	return dest;
}
*/

static __inline__ void memcpy(void *dest, const void *src, size_t n)
{
	int d0, d1, d2;
	__asm__ __volatile__ (
		"cld\t\n"
		"rep\t\n"
		"movsb\t\n"
		:"=&D" (d0), "=&S" (d1), "=&c"(d2)
		:"0"(dest), "1"(src), "2"(n)
		:"memory"
		);
}


/* Memory data transfer between kernel and user */
/* Get a byte from user space */
static inline unsigned char get_user_byte(unsigned char *addr)
{
	unsigned char ret;
	asm volatile (
		"movb %%fs:%1, %0\n\t"
		:"=r"(ret)
		:"m"(*addr)
		);
	return ret;
}

/*
 * Get 2bytes of data from user space
 */
static inline unsigned short get_user_word(const unsigned short *addr)
{
	unsigned short ret;
	asm volatile(
		"movw %%fs:%1, %0\n\t"
		:"=r"(ret)
		:"m"(*addr)
		);
	return ret;
}

/*
 * Get 4bytes of data from user space
 */
static inline unsigned long get_user_long(const unsigned long *addr)
{
	unsigned long ret;
	asm volatile(
		"movl %%fs:%1, %0\n\t"
		:"=r"(ret)
		:"m"(*addr)
		);
	return ret;
}

/*
 * Copy n bytes of memory from user space to kernel space
 */
static inline int usr_to_ker(char *to, const char *from, int n)
{
	int d0, d1, d2;
	asm volatile (
		"pushl %%ds\n\t"
		"pushl %%fs\n\t"
		"popl %%ds\n\t"
		"cld\n\t"
		"rep movsb\n\t"
		"popl %%ds"
		:"=&D" (d0), "=&S" (d1), "=&c"(d2)
		:"0"(to), "1"(from), "2"(n)
		:"memory"
		);

	return n;
}

/*
 * Put a byte to user space
 */
static inline void put_user_byte(unsigned char data, const unsigned char *addr)
{
	asm volatile(
		"movb %0, %%fs:%1\n\t"
		:
		:"r"(data), "m"(*addr)
		);
}

/*
 * Put 2bytes of data to user space
 */
static inline void put_user_word(unsigned short data, const unsigned short *addr)
{
	asm volatile (
		"movw %0, %%fs:%1\n\t"
		:
		:"r"(data),"m"(*addr)
		);
}

/*
 * Put 4bytes of data to user space
 */
static inline void put_user_long(unsigned long data, const unsigned long *addr)
{
	asm volatile (
		"movl %0, %%fs:%1\n\t"
		:
		:"r"(data),"m"(*addr)
		);
}

/*
 * Copy n bytes of memory from kernel space to user space
 */
static inline int ker_to_usr(char *to, const char *from, int n)
{
	int d0, d1, d2;
	asm volatile (
		"pushl %%es\n\t"
		"pushl %%fs\n\t"
		"popl %%es\n\t"
		"cld\n\t"
		"rep movsb\n\t"
		"popl %%es"
		:"=&D" (d0), "=&S" (d1), "=&c"(d2)
		:"0"(to), "1"(from), "2"(n)
		);
	return n;
}

#endif /* __I386_STRING_H  */
