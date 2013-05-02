/*
 * GETTY
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
 * File: getty.c
 * 			A console initializer.
 */


#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <termios.h>

int main(int argc, char *argv[])
{
	int fd;
	pid_t pid;

	pid = setpgrp();
	tcsetpgrp(1, pid);
	printf("Process %d set as a session leader\n", pid);

	fd = open("/dev/tty", O_RDWR);
	if(fd < 0)
	{
		printf("/dev/tty Open Failed. \n");
		return 1;
	}

	execv("/bin/login", NULL);

	return 0;
}
