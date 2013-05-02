/*
 * sh
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
 * File: sh.c
 * 			A Simple Shell.
 */

#include <string.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <termios.h>
#include <stdio.h>


volatile pid_t pid;
char buff[1024];

int main(int argc, char *argv[])
{
	char *ptr;
	int ret, status, ampflag = 1;

	struct termios term;

	printf("\nWelcome to a very simple Shell.\n");

	memset(&term, 0, sizeof(term));
	term.c_lflag = ECHO | ICANON | ISIG;
	term.c_cc[VEOF] = -1;
	term.c_cc[VEOL] = '\n';

	tcsetattr(STDIN_FILENO, TCSANOW, &term);

	do
	{
		printf("\nManRiX > ");
		ret = read(STDIN_FILENO, buff, 254);
		buff[ret] = '\0';
//		if((ptr = strchr(buff, '&')) == NULL)
//			ampflag = 0;
		pid = vfork();
		if(pid == 0)
		{
			ret = execv(buff, NULL);
			if(ret < 0)
				printf("Error Execing %s.\n", buff);
			exit(1);
		}
//		printf("i am %d ", pid);
		ret = wait(&status);
		if(ret < 0)
			printf("Wait failed with status %d ", errno);
		printf("INIT: Child exited with %d.", status);	
	}while(1);

	return 0;
}
