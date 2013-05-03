/**
 * ManRiX Microkernel
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
 **/
/**	file: atomic.h
 *		Arch Dependent Atomic Operations implementation.
 *	SMP safe.
 **/


#ifndef __ASM_ATOMIC_H
#define __ASM_ATOMIC_H

/* Increment a atomically */
static inline int atomic_inc(atomic_t *a)
{
	asm volatile (
		"lock incl %0"
		:"=m"(a->value)
		:"m"(a->value)
		:"memory"
	);
	
	return a->value;
}

/* Decrement a atomically */
static inline int atomic_dec(atomic_t *a)
{
	asm volatile (
		"lock decl %0"
		:"=m"(a->value)
		:"m"(a->value)
		:"memory"
	);
	
	return a->value;
}

/* Add value 'val' to 'a' atomically */
static inline void atomic_add(atomic_t *a, int val)
{
	asm volatile (
		"lock addl %1, %0"
		:"=m"(a->value)
		:"m"(a->value), "r"(val)
		:"memory"
	);
}

/* Subtract value 'val' from 'a' atomically */
static inline void atomic_sub(atomic_t *a, int val)
{
	asm volatile (
		"lock subl %1, %0"
		:"=m"(a->value)
		:"m"(a->value), "r"(val)
		:"memory"
	);
}

#endif /* __ASM_ATOMIC_H */
