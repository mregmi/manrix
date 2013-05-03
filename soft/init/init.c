/*
 * init process for ManRiX
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
 * File: init.c
 * 			This is a initializer process for ManRiX OS. It loads and initialises
 * the servers and services.
 */


#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

int read_entry(char *prog, FILE *fs)
{
	char c = 'c';

	while(c != '\n')
	{
		c = fgetc(fs);
//		printf(" %c ",c);
		if(c == EOF)
			return 1;
		/* skip comments */
		if(c == '#')
		{
			while(fgetc(fs) != '\n') ;
			continue;
		}

		*prog++ = c;
	}

	*prog = '\0';

	return 0;
}

char *get_parm(char *prog)
{
	int i = 0;
	char *c;

	if(!prog)
	{
		printf("prog is NULL ");
		return NULL;
	}
	/* skip first arg */
	while(prog[i++] != ':') ;

//	printf(" %c ", prog[i]);
	/* skip second arg */
	while(prog[i++] != ':') ;

//	printf(" %c ", prog[i]);

	/* skip third arg */
	while(prog[i++] != ':') ;

//	printf(" %c ", prog[i]);

	c = &prog[i];

	while(prog[i] != ' ')
	{
		if(prog[i] == '\0')
			break;
		i++;
	}

	prog[i] = '\0';


	return c;
}

int main(int argc, char *argv[])
{
	int fd, status;
	pid_t child;
	FILE *fd1;

	char *prog;
	char *exec, *parm;

	printf("\nStarting INIT version 1.0 ...");

	/* Start the initial console */
	fd = open("/dev/console", O_RDONLY);
	if(fd < 0)
	{
		printf("Error opening file /dev/console. %d", errno);
		return 1;
	}

	/* Duplicate the console fd for stdout and stderr */
	dup(fd);
	dup(fd);

	fd = open("/etc/inittab", O_RDONLY);
	if(fd < 0)
	{
		printf("Error opening file /etc/inittab. %d", errno);
		return 1;
	}

	/* size of single entry */
/*	prog = (char *)malloc(1024);
	memset(prog, 0, 1024);
	while(read_entry(prog, fd1) == 0)
	{
		parm = get_parm(prog);
//		child = vfork();
//		if(child == 0)
		{
//			printf("i am %d chld = %d execing %s ", getpid(), child, parm);
//			execv(parm, NULL);
			printf("execing %s %d child = %d ", parm, getpid(), child);
//			for(;;);exit(1);
		}
	}
*/
	printf("Init Process Over \n");
	execv("/bin/login", NULL);	
	
//	while(1)
//	{
//		wait(&status);
//		printf("Status is %d\n", status);
//	}

	return 0;
}
