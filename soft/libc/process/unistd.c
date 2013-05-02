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
 * File:- unistd.c
 */

#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <mrx/pager.h>
#include <mrx/fd.h>
#include <mrx/conmngr.h>
#include <asm/ipc.h>
#include <asm/syscall.h>

char **environ = NULL;

/*
 * access - determine accessibility of a file
 */
int access(const char *path, int amode)
{
	struct message msg;
	int fd, length;

	length = strlen(path);
	if(length > MAX_MSG_LONG)
		msg.flags = MSG_MAP_SEND;
	else
		msg.flags = MSG_LONG_SEND;

	msg.snd_ptr = (void *)path;
	msg.snd_bufsize = length;
	msg.w1 = FS_ACCESS;
	msg.w4 = amode;
	send_message(ID_FSIO_MNGR, &msg);

	fd = msg.w1;
	if(fd < 0)
	{
		errno = -fd;
		return -1;
	}

	return 0;
}

/*
 * alarm - schedule an alarm signal
 */
unsigned alarm(unsigned seconds)
{
	return syscall_alarm(seconds);
}

/*
 * chdir - change working directory
 */
int chdir(const char *path)
{
	struct message msg;
	int ret, length;

	length = strlen(path);

	if(length > MAX_MSG_LONG)
		msg.flags = MSG_MAP_SEND;
	else
		msg.flags = MSG_LONG_SEND;


	msg.snd_ptr = (void *)path;
	msg.snd_bufsize = length;
	msg.w1 = FS_CHDIR;
	send_message(ID_FSIO_MNGR, &msg);

	ret = msg.w1;
	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	return 0;
}



/*
 * chown - change owner and group of a file
 */
int chown(const char *path, uid_t owner, gid_t group)
{
	struct message msg;
	int ret, length;

	length = strlen(path);
	if(length > MAX_MSG_LONG)
		msg.flags = MSG_MAP_SEND;
	else
		msg.flags = MSG_LONG_SEND;

	msg.snd_ptr = (void *)path;
	msg.snd_bufsize = length;
	msg.w1 = FS_CHOWN;
	msg.w4 = owner;
	msg.w5 = group;
	send_message(ID_FSIO_MNGR, &msg);

	ret = msg.w1;
	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	return 0;
}

/*
 * close - close a file
 */
int close(int fildes)
{
	struct message msg;
	int fd;

	msg.flags = MSG_SHORT;
	msg.w1 = FS_CLOSE;
	msg.w4 = fildes;
	send_message(ID_FSIO_MNGR, &msg);

	fd = msg.w1;
	if(fd < 0)
	{
		errno = -fd;
		return -1;
	}

	return fd;
}

ssize_t  write(int fildes, const void *buf, size_t nbytes)
{
	struct message msg;
	tid_t server;

	if(nbytes < MAX_MSG_LONG)
		msg.flags = MSG_LONG_SEND;
	else
		msg.flags = MSG_MAP_SEND;

	msg.snd_ptr = (void *)buf;
	msg.snd_bufsize = nbytes;
	msg.w1 = SVR_WRITE;
	msg.w4 = fildes;

	if((fildes >= 0) && (fildes <= 2))
	{
		server = ID_CON_MNGR;
		msg.flags |= MSG_NOREPLY;
	}
	else
	{
		server = FDESC_SVR(fildes);
		if(!server)
			server = ID_FSIO_MNGR;
	}

	send_message(server, &msg);

	if(msg.w1 < 0)
	{
		errno = -msg.w1;
		return -1;
	}

		return msg.w1;
}

ssize_t  read(int fildes, void *buf, size_t nbyte)
{
	struct message msg;
	tid_t server;

	if(nbyte < MAX_MSG_LONG)
		msg.flags = MSG_LONG_RECEIVE;
	else
		msg.flags = MSG_MAP_RECEIVE;
	msg.rcv_ptr = buf;
	msg.rcv_bufsize = nbyte;
	msg.w1 = FS_READ;
	msg.w2 = fildes;

	if((fildes >= 0) && (fildes <= 2))
		server = ID_CON_MNGR;
	else
	{
		server = FDESC_SVR(fildes);
		if(!server)
			server = ID_FSIO_MNGR;
	}

	send_message(server, &msg);

	if(msg.w1 < 0)
	{
		errno = -msg.w1;
		return -1;
	}

	return msg.w1;
}



/*
 * getgid - get real-group-ID
 */
gid_t getgid(void)
{
	struct userid id;

	syscall_get_user(0, &id);

	return id.gid;
}

/*
 * getegid - get the effective group ID
 */
gid_t getegid(void)
{
	struct userid id;

	syscall_get_user(0, &id);

	return id.egid;
}

/*
 * geteuid - get the effective user ID
 */
uid_t geteuid(void)
{
	struct userid id;

	syscall_get_user(0, &id);

	return id.euid;
}

/*
 * getuid - get a real user ID
 */
uid_t getuid(void)
{
	struct userid id;

	syscall_get_user(0, &id);

	return id.uid;
}

/*
 * setegid - set effective-group-ID
 */
int setegid(gid_t egid)
{
	struct userid id;

	id.uid = -1;
	id.euid = -1;
	id.gid = -1;
	id.egid = egid;

	return syscall_set_user(0, &id);
}

/*
 * setgid - set real-group-ID
 */
int setgid(gid_t gid)
{
	struct userid id;

	id.uid = -1;
	id.euid = -1;
	id.gid = gid;
	id.egid = -1;

	return syscall_set_user(0, &id);
}

/*
 * setuid - set real-user-ID
 */
int setuid(uid_t uid)
{
	struct userid id;

	id.uid = uid;
	id.euid = -1;
	id.gid = -1;
	id.egid = -1;

	return syscall_set_user(0, &id);
}

/*
 * seteuid - set effective-user-ID
 */
int seteuid(uid_t euid)
{
	struct userid id;

	id.uid = -1;
	id.euid = euid;
	id.gid = -1;
	id.egid = -1;

	return syscall_set_user(0, &id);
}

/*
 * getpgid - get the process group ID for a process
 */
pid_t getpgid(pid_t pid)
{
	struct userid id;

	syscall_get_user(pid, &id);

	return id.gid;

}

/*
 * getpgrp - get the process group ID of the calling process
 */
pid_t getpgrp(void)
{
	return getgid();
}


/*
 * setpgid - set process group ID for job control
 */
int setpgid(pid_t pid, pid_t pgid)
{
	struct userid id;

	id.uid = -1;
	id.euid = -1;
	id.gid = pgid;
	id.egid = -1;

	return syscall_set_user(pid, &id);

}

/*
 * setsid - create session and set process group ID
 */
pid_t setsid(void)
{
	return setpgid(0, 1);
}

/*
 * setpgrp - set the process group ID
 */
pid_t setpgrp(void)
{
	return setpgid(0, 1);
}

/*
 * getpid - get the process ID
 */
pid_t getpid(void)
{
	return syscall_get_pid(0);
}

/*
 * getppid - get the parent process ID
 */
pid_t getppid(void)
{
	return syscall_get_ppid();
}

/*
 * dup - duplicate an open file descriptor
 */
int dup(int fildes)
{
	return syscall_dup(fildes, 0);
}

/*
 * dup - duplicate an open file descriptor
 */
int dup2(int fildes, int fildes2)
{
	return syscall_dup(fildes, fildes2);
}
