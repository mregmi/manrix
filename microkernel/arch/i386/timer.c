/*
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
 */
/*
 * File:- timer.c
 * Interfacing 8254 chip.
 */
#include <string.h>

#include <kernel/timer.h>
#include <asm/asm.h>
#include <asm/context.h>
#include <kernel/kernel_stdio.h>
#include <kernel/signal.h>
#include <kernel/scheduling.h>

/* Current time */
extern struct timespec ctime;
extern struct timer tmlist;

/* CPU Ticks */
extern volatile u64 ticks;

/* Current Tsc value */
struct tsc ptsc = {0,0};

/* The frequency of the CPU */
volatile u32 cpuspeed = 0;

#define TIMER_GATE	0x20

extern GATE	*idt;
extern void irq_special_tm();
extern void irq_0();

volatile int ndelay = 0;


/* Read the Time stamp counter */
static inline void rdtsc(struct tsc *ts)
{
	asm (
		"rdtsc\n\t"
		:"=a"(ts->low), "=d"(ts->high)
		:
		);
}

/* Read the lower 32-bit of the TSC. */
static inline unsigned long rdtscl()
{
	unsigned long eax;
	asm(
		"rdtsc"
		:"=a"(eax)
		:
	);
	return eax;
}

/*
 * Loop us microseconds and return.
 * Non-blocking delay.
 */
void delay_nb(long us)
{
	unsigned long old, new;

	old = rdtscl();
	new = old + us * cpuspeed;

	while(rdtscl() < new)
		;
}

/* special Interrupt handler for Delaying */
void special_int()
{
	//kprintf(" %d ", ndelay);
	ndelay--;
}

/*
 * This is a special function for delaying.
 */
static inline void delay(int secs)
{
	u16 hbckup, lbckup;

	/* save the original idt descriptor offset for the Timer HW interrupt */
	hbckup = idt[TIMER_GATE].offset_high;
	lbckup = idt[TIMER_GATE].offset_low;

	/* Load the special handler for the Timer Chip. */
	set_interrupt_gate(TIMER_GATE, &irq_special_tm);

	/* Ticks to Delay */
	ndelay = secs * HZ;

	STI();

	/* Loop till Time Elapse. */
	while(ndelay > 0)
		asm volatile ("nop");

	CLI();

	/* Reload Original Handler */
	set_interrupt_gate(TIMER_GATE, &irq_0);
}

/*
 * Get the Speed of the CPU.
 */
u32 get_cpu_speed()
{
	u32 speed;
	struct tsc first, second;

	rdtsc(&first);
	delay(1);
	rdtsc(&second);
	asm(
	     "subl %2, %0\n\t"
	     "sbbl %3, %1\n\t"
	     :"=r"(second.low),"=r"(second.high)
	     :"g"(first.low),"g"(first.high), "0"(second.low),"1"(second.high)
	     );

	asm (
	  	"divl %1"
		:"=a"(speed)
		:"r"(1000),"d"(second.high),"a"(second.low)
	);

	return speed;
}

/* Initialize Timer chip(8254). */
void timer_init()
{
	unsigned char low, high;

	low = INTERVAL % 256;
	high = INTERVAL / 256;

	outportb(RATE_GEN|MSBLSB, CONTROL);
	outportb(low, CHANNEL0);
	outportb(high, CHANNEL0);

	cpuspeed = get_cpu_speed();

	kprintf("CPU Speed %d MHZ. ", cpuspeed);
	rdtsc(&ptsc);
}

time_t mktime(u32 seconds, u32 minutes, u32 hours, u32 dom, u32 month, u32 year)
{
	/* From SUS V3 */
	return ( seconds + \
			(minutes * 60) + \
			(hours * 3600) + \
			(dom * 86400) + \
			((year - 70) * 31536000) + \
			(((year - 69)/4) * 86400) - \
			(((year - 1)/100) * 86400) + \
			(((year + 299)/400) * 86400) \
			);
}

/* Get current time from CMOS */
time_t get_time()
{
	time_t time;
	u32 seconds, minutes, hours, day_of_month, month, year, centuary;

	/* Check if we are ready to READ CMOS */
	outportb(RTC_STATUS_A, RTC_ADDRESS);
	while(inportb(RTC_DATA) & 0x80)
		;

	seconds = BIN(READ_CMOS(RTC_SECONDS));
	minutes = BIN(READ_CMOS(RTC_MINUTES));
	hours	= BIN(READ_CMOS(RTC_HOURS));
	day_of_month = BIN(READ_CMOS(RTC_DAY_OF_MONTH));
	month	= BIN(READ_CMOS(RTC_MONTH));
	year	= BIN(READ_CMOS(RTC_YEAR));
	centuary= BIN(READ_CMOS(RTC_CENTUARY));	/* Does all CMOS support this???? */

	year=(centuary * 100)+year;
	kprintf(" %d:%d:%d %d/%d/%d ",hours,minutes,seconds,day_of_month,month,year);
	year= year-1970;
	time = mktime(seconds, minutes, hours, day_of_month, month, year);
	return time;
}

/* Get the current time */
int do_gettime(struct timespec *tm)
{
	struct tsc new;
	u32 time;

	rdtsc(&new);
	asm(
	     "subl %2, %0\n\t"
	     "sbbl %3, %1\n\t"
	     :"=r"(new.low),"=r"(new.high)
	     :"g"(ptsc.low),"g"(ptsc.high), "0"(new.low),"1"(new.high)
     );

	/* Update current TSC value */
	rdtsc(&ptsc);

	asm (
	  	"divl %1"
		:"=a"(time)
		:"r"(cpuspeed),"d"(new.high),"a"(new.low)
	);

	ctime.tv_nsec += (time%1000000)*1000;
	ctime.tv_sec += time/1000000;
	if(ctime.tv_nsec >= 1000000000)
	{
		ctime.tv_nsec %= 1000000000;
		ctime.tv_sec += (ctime.tv_sec/1000000000);
	}
	tm = &ctime;

	return 0;
}

/* Set the current time */
void do_settime(struct timespec *tm)
{
	ctime.tv_sec = tm->tv_sec;
	ctime.tv_nsec = tm->tv_nsec;
}

unsigned syscall_alarm(struct stack_regs *regs)
{
	timer_t tm;
	struct sigevent seg;
	process_t proc = current->proc;

	unsigned long time_ret = (proc->alarm->timeout - ticks);

	if(proc->alarm)
		return (time_ret/1000000000);
	if(!regs->ebx)
	{
		delete_timer(proc->alarm->timer_id);
		proc->alarm = NULL;
	}

	seg.sigev_signo = SIGALRM;
	tm = timer_create(CLOCK_REALTIME, TM_TIMER|TM_ONESHOT, &seg);
	if((int)tm < 0)
		return (int)tm;

	tm->timeout = ticks + (regs->ebx * 1000000000);
	tm->nextarm = 0;
	insert_timer(&tmlist, tm);

	return 0;
}

/* Syscall to get the resolution of specfied clock */
int syscall_getresolution(struct stack_regs *regs)
{
	struct timespec tmspec;
	int ret;

	ret = getresolution(regs->ebx, &tmspec);
	regs->ebx = tmspec.tv_sec;
	regs->ecx = tmspec.tv_nsec;

	return ret;
}

int syscall_nsleep(struct stack_regs *regs)
{
	int ret;
	struct timespec *rqtp, *rmtp;
	rqtp = (struct timespec *)(regs);
	rmtp = (struct timespec *)((int *)regs + 4);

	ret = do_nsleep(rqtp, rmtp);

	return ret;
}

int syscall_timer_create(struct stack_regs *regs)
{
	struct sigevent evp;
	timer_t tm;

	usr_to_ker((void *)&evp, (void *)regs->edi, sizeof(struct sigevent));

	tm = timer_create(regs->ebx, TM_TIMER, &evp);
	if((int)tm < 0)
		return (int)tm;

	return tm->timer_id;
}

int syscall_timer_delete(struct stack_regs *regs)
{
	int ret;
	ret = delete_timer(regs->ebx);

	return ret;
}

int syscall_get_timer(struct stack_regs *regs)
{
	timer_t tm;
	struct itimerspec itm = {{0},{0}};

	tm = timer_by_id(regs->ebx);
	if(!tm)
		return -EINVAL;
	if(tm->timeout)
		tick_to_timespec(&itm.it_value, tm->timeout);
	if(regs->ecx)
		ker_to_usr((void *)regs->ecx, (void *)&itm, sizeof(struct itimerspec));

	return 0;
}

int syscall_set_timer(struct stack_regs *regs)
{
	return 0;
}


/* get the current time */
int syscall_gettime(struct stack_regs *regs)
{
	struct timespec time;
	if(regs->ebx != CLOCK_REALTIME)
		return -EINVAL;

	do_gettime(&time);

	regs->ebx = time.tv_sec;
	regs->ecx = time.tv_nsec;

	return time.tv_sec;
}

int syscall_settime(struct stack_regs *regs)
{
	struct timespec time;

	if(regs->edx != CLOCK_REALTIME)
		return -EINVAL;

	time.tv_sec = regs->ebx;
	time.tv_nsec = regs->ecx;
	do_settime(&time);

	return 0;
}


int syscall_clock(struct stack_regs *regs)
{
	struct tms tms, *tmsp = NULL;

	if(regs->ebx)
	{
		tmsp = &tms;
		usr_to_ker((void *)tmsp, (void *)regs->ebx, sizeof(struct tms));
	}

	return get_clock(tmsp);
}

