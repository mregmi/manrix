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
/**
 * File:- timer.h
 *	The Architecture Specfic Timer declarations.
 **/
#ifndef __ASM_TIMER_H
#define __ASM_TIMER_H

#include <time.h>

#include <asm/asm.h>
#include <kernel/types.h>

#define DEFAULT_HZ			0x1234DD

#define HZ				1000

#define INTERVAL			(DEFAULT_HZ/HZ)

#define CHANNEL0			0x40	/* TIMER */
#define CHANNEL1			0x41	/* Ram refresh. Do not touch */
#define CHANNEL2			0x42	/* Internal Speaker */
#define CONTROL				0x43	/* Control Word */

#define BCD_SELECT			0x01
#define RATE_GEN			0x04
#define MSBLSB				0x30
#define SELECT_C2			0x80


#define RTC_ADDRESS			0x70
#define RTC_DATA			0x71

#define RTC_STATUS_A		0x0A
#define RTC_STATUS_B		0x0B
#define RTC_STATUS_C		0x0C
#define RTC_STATUS_D		0x0D

#define RTC_SECONDS			0x00
#define RTC_MINUTES			0x02
#define RTC_HOURS			0x04
#define RTC_DAY_OF_MONTH	0x07
#define RTC_MONTH			0x08
#define RTC_YEAR			0x09
#define RTC_CENTUARY		0x32

/*
 * The 64bit Time Stamp Counter.
 */
struct tsc{
	u32 low;
	u32 high;
};

/*
 * Convert the BCD value to Binary Value.
 */
#define BIN(value)		(((value)&0xF)+(((value)>>4)*10))

/*
 * Convert the Binary value to BCD Value.
 */
#define BCD(value)		((((value)/10)<<4)+((value)%10))

/*
 * Read the CMOS chip at given Offset.
 */
static inline u8 READ_CMOS(u8 offset)
{
	register u8 data;
	outportb(offset, RTC_ADDRESS);
	data = inportb(RTC_DATA);
	return data;
}

/* Get the current time */
extern int do_gettime(struct timespec *tm);
extern void timer_init();
extern time_t get_time();
extern void delay_nb(long );

extern int do_gettime(struct timespec *);
extern void do_settime(struct timespec *);
extern int getresolution(clockid_t , struct timespec *);

#endif  /* __I386_TIMER_H  */
