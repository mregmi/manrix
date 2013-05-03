/*
 * Libc for ManRiX OS
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
 * File: file.c
 *		File handling functions.
 */

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <errno.h>

#include <asm/ipc.h>
#include <asm/syscall.h>
#include <mrx/fd.h>
#include <mrx/conmngr.h>

off_t lseek(int fildes, off_t offset, int whence)
{
	struct file file;
	int ret;
	pthread_t tid = pthread_self();

	ret = get_openfile(tid, fildes, &file);
	if(ret < 0)
	{
		__set_errno(-ret);
		return -1;
	}

	if(whence == SEEK_SET)
		file.f_offset = offset;
	else if(whence == SEEK_CUR)
		file.f_offset += offset;
	else if(whence == SEEK_END)
		file.f_offset = -1;
	else
	{
		__set_errno(EINVAL);
		return -1;
	}

	ret = put_openfile(tid, fildes, &file);

	return file.f_offset;
}
