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
 * File:- kernel_stdio.c
 * 		This file contains implimentations for standard input and
 * output functions.
 *
 **/

#include <kernel/config.h>

#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include <kernel/kernel_stdio.h>

#include <asm/console.h>

char buf[BUFSIZE] = {[0 ... BUFSIZE-1] = 0};
char logbuf[LOGBUFSIZE] = {[0 ... BUFSIZE-1] = 0};
static int end = 0;

int init = 0;

/* convert attributes to string */
static int ctrl_to_str(char *buff, int base, int flag, unsigned int num)
{
	unsigned int tmp;
	int count = 0, ret;
	char tmps[36], *digits;
	char sign;
	char lower[] = {"0123456789abcdef"};
	char upper[] = {"0123456789ABCDEF"};

	if(flag & CASE)
		digits = upper;
	else
		digits = lower;

	if(num == 0)
		tmps[count++] = '0';

	while(num != 0)
	{
		tmp = num % base;
		num = num / base;
		tmps[count] = digits[tmp];
		count++;
	}

	if((flag & SIGNED) && (num < 0))
	{
		sign = '-';
		num = -num;
		tmps[count] = sign;
	}

	ret = count;

	while(count--)
	{
		*buff++ = tmps[count];
	}

	return ret;
}


/* Create a formatted output and store in buff */
/* Linux source was very much helpful (copyright Linus Trovaldis and others) */
int vsprintf(char *buff, const char *fmt, va_list ap)
{
	int index = 0, base = 0, count =0, ctrl;
	int flag = 0;
	char qualifier = 'l';
	char *tmp;
	unsigned int longn;

	for(index = 0;fmt[index] != '\0'; index++)
	{
		if(fmt[index] != '%')
		{
			*buff++ = fmt[index];
			count++;
			continue;
		}

		index++;

		/* qualifier */
		if(fmt[index] == 'l')
		{
			qualifier = 'l';
			index++;
		}
		else if(fmt[index] == 'L')
		{
			qualifier = 'L';
			index++;
		}

		/* now get the base */
		switch(fmt[index])
		{
			case 'c':
					*buff++ = va_arg(ap, char);
					count++;
					break;

			case 's':
					tmp = va_arg(ap, char *);
					if(!tmp)
						tmp = "(null)";
					while(*tmp != '\0')
					{
						*buff++ = *tmp++;
						count++;
					}
					break;

			case '%':
					*buff++ = '%';
					count++;
					break;

			case 'X':
					flag |= CASE;
			case 'x':
					base = 16;
					longn = va_arg(ap, int);
					ctrl = ctrl_to_str(buff, base, flag, longn);
					buff += ctrl; count += ctrl;
					break;
			case 'd':
					base = 10;
					if(qualifier == 'l')
						longn = va_arg(ap, long);
					else if(qualifier == 'L')
						longn = va_arg(ap, long long);
					else
					{
						flag |= SIGNED;
						longn = va_arg(ap, int);
					}

					ctrl = ctrl_to_str(buff, base, flag, longn);
					buff += ctrl; count += ctrl;
					break;

			case 'u':
					base = 10;
					longn = va_arg(ap, unsigned long);
					ctrl = ctrl_to_str(buff, base, flag, longn);
					buff += ctrl; count += ctrl;
					break;
		}
	}
	*buff = '\0';
	return count;
}


/* used to print in kernel mode */
int kprintf(const char *format, ...)
{
	va_list ap;
	int ret;

	va_start(ap, format);
	ret = vsprintf(buf, format, ap);

	va_end(ap);

//	if(!init)
		console_write(buf);

	if(end+ret > LOGBUFSIZE)
		end = 0;

	memcpy(&logbuf[end], buf, ret);

	return ret;
}

/* Encountered the a serious problem */
void panic(const char *fmt, ...)
{
	kprintf(fmt);

	asm("cli;");
	for(;;);
}

int syscall_log(void *ptr, void *data)
{
	int sz;

	kprintf("logging");
	if(data)
		ker_to_usr(data, (void *)&logbuf, end);

	sz = end;
	end = 0;

	return sz;
}

