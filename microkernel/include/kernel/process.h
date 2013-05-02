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
 * File:- process.h
 * 	The Macros and Structures for Process Managment.
 */

#ifndef __KERNEL_PROCESS_H
#define __KERNEL_PROCESS_H

#include <sys/types.h>

#include <kernel/vm.h>
#include <kernel/list.h>
#include <kernel/error.h>
#include <kernel/signal.h>
#include <kernel/timer.h>
#include <kernel/sync.h>
#include <kernel/spinlock.h>
#include <kernel/atomic.h>

/* Process Flags */
#define DUP_ALL		1	/* Duplicate everyting Equal to FORK */
#define DUP_SHARE	2	/* Dont Duplicate but SHARE; EQUAL to VFORK */
#define DUP_NONE	4	/* Dont Duplicate anything Equal to SPAWN */

/* Process Status */
#define PROCESS_ACTIVE	1
#define PROCESS_ZOMBIE	2
#define ON_SIGHAND		4
#define STAT_VFORKED	8

#define WNOHANG			1
#define WAIT_THREAD		2

#define WEXIT			0x0000FF
#define WSIGNAL			0x00FF00
#define WIFEXIT			0x010000
#define WIFSIG			0x020000


/* No of reserved pids. */
#define FIRST_USER_PID		500

#ifndef __PID_T
#define __PID_T
typedef int pid_t;
#endif

/*
 * The File Descriptor is implemented in MicroKenrel.
 */
/* Default no of fd */
#define MAX_DEF_FD		32
#define MAX_PATH_LEN		256

/* Each file has unique device, inode combination. */
struct inodev{
	ino_t inode;
	dev_t device;
};

/*
 * the Open File Table.
 */
struct file{
	mode_t  f_access;	/* opened mode */
	off_t	f_offset;	/* offset in the file. */
	int 	f_ref;		/* reference count */

	struct inodev vnode;
};

struct fd{
	int			fd_max;
	int			fd_hint;
	unsigned int bitmap;

	/* Current working Directory info. */
	struct inodev cwd;
	char cwdstr[MAX_PATH_LEN];

	/*
	 * We use slab allocation for open file table.
	 * fileptr contains the array of pointers to the open file table.
	 */
	vm_offset_t fileptr[MAX_DEF_FD + 1];

	spinlock_t lock;

};

/*
 * the Process Structure.
 */
struct process {
	struct linked_list	threads;	/* list of threads belonging to this process. */
	struct linked_list	child;		/* List of its children */
	struct linked_list  proclist;	/* List of all processes. */
	struct vm_map		*map;		/* vm_map of a process. */
	pid_t		pid;		/* Our Pid */

	uid_t uid, euid;		/* users real and effective user ids owning this process */
	gid_t gid, egid;		/* users real and effective group ids owning this process */
	pid_t session;

	struct process	*parent;	/* Pointer to parent's Proc */

	struct process	*next;		/* hash link */

	long start_time;		/* The start time of process */
	long user_time;			/* Time spent in user space */
	long system_time;		/* Time spent in Kernel */
	struct linked_list timer;	/* List of timers */
	struct timer *alarm;

	int flag;			/* Flags (see above) */
	int status;			/* status (see above) */
	int exit_status;

	struct linked_list sigqueue;	/* signal queue head */
#define nextq	sigqueue.next
#define prevq	sigqieue.prev

	struct sigaction action[MAX_SIGNAL];	/* Signal action */
	spinlock_t siglock;			/* Lock for Signal */	

	/* Alternate stack for signal handling */
	stack_t ss;

	struct syncstruct sync;	/* List of allocated Sync */

	/* We can put open file table in Kernel.*/

	struct fd fd;

	spinlock_t lock;		/* Lock for Process structure */
};

typedef struct process *process_t;

#define have_threads(p)		(!list_isempty(&p->threads))
#define have_children(p)	(!list_isempty(&p->child))

/* Get the main thread of the process */
static inline thread_t get_main_thread(process_t p)
{
	thread_t thr;
	last_entry(&p->threads, thr);

	return thr;
}

/* Get the user id */
static inline uid_t get_uid(process_t p)
{
	return p->uid;
}

/* Get the user id */
static inline uid_t get_euid(process_t p)
{
	return p->euid;
}

/* Get the user id */
static inline gid_t get_gid(process_t p)
{
	return p->gid;
}

/* Get the user id */
static inline gid_t get_egid(process_t p)
{
	return p->egid;
}

/* Set the group id */
static inline void set_pgrp(process_t p)
{
	p->gid = p->pid;
	p->egid = p->pid;

	if(p->session != p->pid)
		p->session = p->pid;
}

/* Set the user id */
static inline void set_uid(process_t p, uid_t uid)
{
	p->uid = uid;
	p->euid = uid;
}

/* Set the effective user id */
static inline void set_euid(process_t p, uid_t euid)
{
	if((euid == p->uid) || superuser())
		p->euid = euid;
}

/* Set the effective group id */
static inline void set_egid(process_t p, gid_t egid)
{
	if((egid == p->gid) || superuser())
		p->egid = egid;
}

extern void process_init();

/* Find a process by its pid. */
extern process_t find_by_pid(pid_t );

/* Create a process */
extern process_t do_fork(process_t , u32 );
extern process_t do_spawn(process_t , struct thread_attr *);
/*
 * According to POSIX,
 * Dupicate only that thread that called fork
 */
extern error_t dup_thread(process_t );

/* Wait till child exits */
extern pid_t waitpid(pid_t , int *, int );
extern clock_t get_childs_utime(process_t );
extern clock_t get_childs_stime(process_t );

extern int syscall_exit(void *, int );
extern void  change_pid(process_t , pid_t );
extern int exec_kludge(struct vn_object *, vm_offset_t );
extern void arch_process_destroy();

extern int fd_fork(process_t , process_t );
extern void fd_free(struct fd *);
extern int alloc_fdesc(struct fd *);
extern int free_fdesc(struct fd *, int );
extern int grow_fd(struct fd *, int );
extern void fd_init();


#endif /* __KERNEL_PROCESS_H */
