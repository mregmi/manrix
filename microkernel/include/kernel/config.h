/**
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
 **/
/**
 * File: config.h
 *		The Compile time configurations
 **/

#ifndef __KERNEL_CONFIG_H
#define __KERNEL_CONFIG_H

#define LOG_INFO				1
#define LOG_ERR					2
#define LOG_ALL					3

#define MANRIX_DEBUG			1
#ifdef MANRIX_DEBUG
 #define LOGLEVEL			LOG_ALL
#endif /* MANRIX_DEBUG */

//#define MANRIX_SMP			1
//#define MANRIX_KPREEMPT			1
//#define MANRIX_IOAPIC			1
//#define MANRIX_LAPIC			1

#if defined(MANRIX_SMP) && !defined(MANRIX_IOAPIC)
 #error "You cannot configure your system as SMP without IOAPIC"
#endif

#if defined(MANRIX_IOAPIC) && !defined(MANRIX_LAPIC)
 #error "You cannot configure your system with IOAPIC but without LAPIC"
#endif

#ifdef MANRIX_SMP
 #define MAX_CPU			8
#else
 #define MAX_CPU			1
#endif /* MANRIX_SMP */

#endif /* __CONFIG_H */
