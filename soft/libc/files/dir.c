/*
 * Libc for ManRix OS
 * Copyright (C) 2004, 2005	Manish Regmi (regmi dot manish at gmail dot com)
 *				Rajesh Bikram R.C   (rajee5283 at hotmail dot com)
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
 */
/*
 * File: dir.c
 * 		Directory related calls.
 */

#include <pthread.h> 
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include <asm/ipc.h>
#include <asm/syscall.h>

/*
 * getcwd - get the pathname of the current working directory
 */
 char *getcwd(char *buf, size_t size)
{
	int ret;
	
	if(!size)
	{
		__set_errno(EINVAL);
		return NULL;
	}
	ret = syscall_get_cwd(pthread_self(), NULL, buf);
	if(ret < 0)
	{
		__set_errno(-ret);
		return NULL;
	}
	
	return buf;	
}
