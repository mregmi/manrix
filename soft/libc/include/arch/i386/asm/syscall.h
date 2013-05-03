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
 * It handles microkernel system calls
 */

#ifndef __ASM_SYSCALL_H
#define __ASM_SYSCALL_H

#include <time.h>
#include <errno.h>
#include <asm/syscalldefs.h>
#include <mrx/fd.h>



static inline int syscall_fd_alloc(tid_t tid, int desc, struct file *table)
{
	register int ret;
	asm (
		"int $0x30"
		:"=a"(ret)
		:"0"(SYS_FD_ALLOC), "b"(tid), "c"(desc), "d"(table)
		);
	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	return ret;
}

static inline int syscall_fd_free(tid_t tid, int desc, struct file *table)
{
	register int ret;
	asm (
		"int $0x30"
		:"=a"(ret)
		:"0"(SYS_FD_FREE), "b"(tid), "c"(desc), "d"(table)
		);
	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	return ret;
}

static inline int syscall_dup(int fd1, int fd2)
{
	register int ret;
	asm (
		"int $0x30"
		:"=a"(ret)
		:"0"(SYS_DUP), "D"(fd1), "S"(fd2)
		);
	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	return ret;
}

static inline int syscall_sched_get(pid_t pid, struct sched_param *parm, int flag)
{
	register int policy;
	asm volatile  (
		"int $0x30\n\t"
		:"=a"(policy)
		:"0"(SYS_SCHED_GET),"b"(pid),"c"(parm), "d"(flag)
		);
	return policy;
}

static inline int syscall_sched_set(pid_t pid, int policy,
			const struct sched_param *parm, int flag)
{
	register int ret;
	asm volatile  (
		"int $0x30\n\t"
		:"=a"(ret)
		:"0"(SYS_SCHED_SET),"b"(pid),"c"(policy),"d"(parm),"D"(flag)
		);
	return ret;
}

static inline int syscall_sched_yield()
{
	register int ret;
	asm volatile  (
		"int $0x30\n\t"
		:"=a"(ret)
		:"0"(SYS_SCHED_YIELD)
		);
	return ret;
}

static inline int syscall_fork()
{
	int pid;
	asm volatile  (
		"int $0x30\n\t"
		:"=a"(pid)
		:"0"(SYS_FORK)
		);
	return pid;
}

static inline pid_t syscall_vfork()
{
	int pid;
	__asm__ __volatile__  (
		"int $0x30\n\t"
		:"=a"(pid)
		:"0"(SYS_VFORK)
		);
	return pid;
}

#define vfork syscall_vfork

static inline int syscall_sleep_unint(tid_t tid)
{
	register int ret;
	asm volatile (
		"int $0x30\n\t"
		:"=a"(ret)
		:"0"(SYS_SLEEP_UNINT), "b"(tid)
		);
	return ret;
}

static inline int syscall_wake_unint(tid_t tid)
{
	register int ret;
	asm volatile(
		"int $0x30\n\t"
		:"=a"(ret)
		:"0"(SYS_WAKE_UNINT), "b"(tid)
		);
	return ret;
}

static inline int syscall_get_pid(tid_t tid)
{
	register int pid;
	asm volatile  (
		"int $0x30\n\t"
		:"=a"(pid)
		:"0"(SYS_GET_PID), "D"(tid)
		);
	return pid;
}


static inline int syscall_get_ppid()
{
	register int ppid;
	asm volatile  (
		"int $0x30\n\t"
		:"=a"(ppid)
		:"0"(SYS_GET_PPID)
		);
	return ppid;
}

static inline int syscall_get_tid()
{
	register int tid;
	asm volatile  (
		"int $0x30\n\t"
		:"=a"(tid)
		:"0"(SYS_GET_TID)
		);
	return tid;
}

static inline int syscall_set_user(pid_t pid, struct userid *id)
{
	register int ret;
	asm volatile  (
		"int $0x30\n\t"
		:"=a"(ret)
		:"0"(SYS_SET_USER), "b"(pid), "c"(id->uid), "d"(id->euid), "S"(id->gid), "D"(id->egid)
		);
	return ret;
}

static inline int syscall_get_user(pid_t pid, struct userid *id)
{
	register int uid;
	asm volatile  (
		"int $0x30\n\t"
		:"=a"(id->uid), "=b"(id->euid), "=c"(id->gid), "=d"(id->egid)
		:"0"(SYS_GET_USER), "1"(pid)
		);
	return uid;
}


static inline void syscall_exit()
{
	register int tid;
	asm volatile  (
		"int $0x30\n\t"
		:"=a"(tid)
		:"0"(SYS_EXIT)
		);
}

#include <mrx/pager.h>

static inline void syscall_exec(void *eip, struct vn_object *ob, void *argv, void *envp)
{
	int d0;
	asm volatile  (
		"int $0x30\n\t"
		:"=a"(d0)
		:"0"(SYS_EXEC), "b"(eip), "c"(ob), "d"(argv), "S"(envp)
		);
}

static inline int syscall_create_thread(struct pthread_attr *attr, void (*start_routine)(void))
{
	register int tid;
	__asm__ __volatile__(
		"int $0x30\n\t"
		:"=a"(tid)
		:"0"(SYS_CREATE_THREAD), "b"(attr), "c"(start_routine)
		);
	return tid;
}

static inline int syscall_destroy_thread(int val)
{
	register int ret;
	asm volatile  (
		"int $0x30\n\t"
		:"=a"(ret)
		:"0"(SYS_DESTROY_THREAD), "b"(val)
		);
	return ret;
}

static inline unsigned long syscall_get_ustack()
{
	register unsigned long ret;
	asm volatile  (
		"int $0x30\n\t"
		:"=a"(ret)
		:"0"(SYS_GET_USTACK)
		);
	return ret;
}

static inline unsigned long syscall_wait(pid_t pid, int *statloc, int flags)
{
	register unsigned long ret;
	asm volatile  (
		"int $0x30\n\t"
		:"=a"(ret), "=c"(*statloc)
		:"0"(SYS_WAIT), "b"(pid), "1"(*statloc), "d"(flags)
		);
	return ret;
}

static inline int syscall_thread_join(pthread_t thread, int *val)
{
	register int ret;
	asm volatile  (
		"int $0x30\n\t"
		:"=a"(ret), "=c"(* val)
		:"0"(SYS_THREAD_JOIN), "b"(thread), "1"(*val)
		);
	return ret;
}

static inline int syscall_nanosleep(const struct timespec *rqtp, struct timespec *rmtp)
{
	register int ret;
	unsigned long sec, nsec;

	asm volatile (
		"int $0x30\n\t"
		:"=a"(ret), "=b"(sec), "=d"(nsec)
		:"0"(SYS_NSLEEP),"D"(rqtp->tv_sec),"S"(rqtp->tv_nsec)
		);

	if(!rmtp)
		return ret;

	rmtp->tv_sec = sec;
	rmtp->tv_nsec = nsec;

	return ret;
}

static inline int syscall_getresolution(clock_t clockid, struct timespec *tm)
{
	register int ret;
	asm volatile (
		"int $0x30\n\t"
		:"=a"(ret)
		:"0"(SYS_GET_RESOLUTION),"b"(clockid),"c"(tm)
		);
	return ret;
}

static inline int syscall_timer_create(clockid_t clockid, int flag,struct sigevent *__restrict evp, timer_t *__restrict timerid)
{
	register int ret;
	asm volatile (
		"int $0x30\n\t"
		:"=a"(ret)
		:"0"(SYS_TIMER_CREATE),"b"(timerid),"c"(clockid),"d"(flag),"S"(evp)
	   );
	return ret;
}

static inline int syscall_timer_delete(timer_t timerid)
{
	register int ret;
	asm volatile (
		"int $0x30\n\t"
		:"=a"(ret)
		:"0"(SYS_TIMER_DELETE),"b"(timerid)
		);
	return ret;
}

static inline int syscall_timer_gettime(timer_t timerid, struct itimerspec *value)
{
	register int ret;
	asm volatile (
		"int $0x30\n\t"
		:"=a"(ret)
		:"0"(SYS_GET_TIMER),"b"(timerid),"c"(value)
		);
	return ret;
}

static inline int syscall_alarm(unsigned seconds)
{
	register int ret;
	asm volatile (
		"int $0x30\n\t"
		:"=a"(ret)
		:"0"(SYS_ALARM),"b"(seconds)
		);
	return ret;
}

#include <sys/times.h>
static inline int syscall_clock(struct tms *tms)
{
	register unsigned long ret;
	asm volatile (
		"int $0x30\n\t"
		:"=a"(ret)
		:"0"(SYS_CLOCK),"b"(tms)
		);
	return ret;
}

static inline int syscall_gettime(clockid_t id, struct timespec *tm)
{
	unsigned long  sec, nsec;
	int ret;

	asm volatile (
		"int $0x30\n\t"
		:"=a"(ret), "=b"(sec), "=c"(nsec)
		:"0"(SYS_GETTIME),"1"(id)
		);
	if(!tm)
		return ret;

	tm->tv_sec = sec;
	tm->tv_nsec = nsec;

	return ret;
}

static inline int syscall_settime(clockid_t id, const struct timespec *tm)
{
	int ret;

	asm volatile (
		"int $0x30\n\t"
		:"=a"(ret)
		:"0"(SYS_SETTIME),"b"(tm->tv_sec), "c"(tm->tv_nsec), "d"(id)
		);

	return ret;
}
/*extern int send_message(tid_t tid, message_t msg);

extern int receive_message(tid_t tid, message_t msg);

extern int reply_message(tid_t tid, message_t msg);
*/

/* Wait for IRQ */
static inline int syscall_IRQ_wait()
{
	register int ret;

	asm volatile(
		"int $0x30"
		:"=a"(ret)
		:"0"(SYS_IRQ_WAIT)
		);

	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	return ret;
}

/* Request for an IRQ handler */
static inline int syscall_IRQ_request(int irq)
{
	register int ret;

	asm volatile (
		"int $0x30"
		:"=a"(ret)
		:"0"(SYS_IRQ_REQUEST), "D"(irq)
		);

	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	return ret;
}

/* Free an IRQ handler */
static inline int syscall_IRQ_free(int id)
{
	register int ret;

	asm (
		"int $0x30"
		:"=a"(ret)
		:"0"(SYS_IRQ_FREE), "D"(id)
		);

	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	return ret;
}

static inline int syscall_sync_create(int type)
{
	register int ret;
	asm volatile (
		"int $0x30\n\t"
		:"=a"(ret)
		:"0"(SYS_SYNC_CREATE),"b"(type)
		);
	return ret;
}

static inline int syscall_sync_destroy(int id)
{
	register int ret;
	asm volatile (
		"int $0x30\n\t"
		:"=a"(ret)
		:"0"(SYS_SYNC_DESTROY),"b"(id)
		);
	return ret;
}

static inline int syscall_sync_lock(int id)
{
	register int ret;
	asm volatile (
		"int $0x30\n\t"
		:"=a"(ret)
		:"0"(SYS_SYNC_LOCK),"b"(id)
		);
	return ret;
}

static inline int syscall_sync_unlock(int id)
{
	register int ret;
	asm volatile (
		"int $0x30\n\t"
		:"=a"(ret)
		:"0"(SYS_SYNC_UNLOCK),"b"(id)
		);
	return ret;
}

static inline int syscall_get_cwd(tid_t tid, struct indev *indev, char *path)
{
	register int ret;
	asm volatile (
		"int $0x30\n\t"
		:"=a"(ret)
	:"0"(SYS_GET_CWD),"b"(tid), "c"(indev), "d"(path)
		);
	return ret;
}

static inline int syscall_put_cwd(tid_t tid, struct indev *indev, char *path)
{
	register int ret;
	asm volatile (
		"int $0x30\n\t"
		:"=a"(ret)
		:"0"(SYS_PUT_CWD),"b"(tid), "c"(indev), "d"(path)
		);
	return ret;
}

static inline int syscall_log(void *ptr)
{
	register int ret;
	asm volatile (
		"int $0x30\n\t"
		:"=a"(ret)
		:"0"(SYS_LOG),"D"(ptr)
		);
	return ret;
}

#include <sys/sysinfo.h>

static inline int syscall_sysinfo(struct sysinfo *info)
{
	register int ret;
	asm volatile (
		"int $0x30\n\t"
		:"=a"(ret)
		:"0"(SYS_SYSINFO),"D"(info)
		);
	return ret;
}

static inline int syscall_change_iopl(int iopl)
{
	register int ret;
	asm volatile (
		"int $0x30\n\t"
		:"=a"(ret)
		:"0"(SYS_CHANGE_IOPL),"b"(iopl)
		);
	return ret;
}

#endif /* __ASM_SYSCALL_H */

