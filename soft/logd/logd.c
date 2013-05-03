/*
 * LOGGER
 * Copyright (C) 2004, 2005	Manish Regmi (regmi dot manish at gmail dot com)
 *				            Rajesh R.C (rajee5283 at hotmail dot com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*
 * File: logd.c
 * 			This is a small utility to dump messages. It receives messages
 * from Kernel, FS manager, Drivers, and Consol Manager.
 */


#include <time.h>
#include <string.h>
#include <stdio.h>

#include <asm/syscall.h>

int main(int argc, char *argv[])
{
	struct timespec rqtp;
	int sz = 10;
	char *data;

	rqtp.tv_sec = 5;
	rqtp.tv_nsec = 0;

	data = (char *)malloc(4096);
	printf("Logger Initializing.....\n");
	while(1)
	{
		sz = syscall_log(data);
		if(sz < 0)
			goto slp;

		printf("%s\n",data);
slp:
		printf("slp %d", sz);
		nanosleep(&rqtp, NULL);
	}

	return 0;
}
