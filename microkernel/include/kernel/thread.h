/**
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
 **/
/**
 * File: thread.h
 * Threading primitives
 **/

#ifndef __KERNEL_THREAD_H
#define __KERNEL_THREAD_H

#include <signal.h>
#include <kernel/config.h>
#include <kernel/list.h>
#include <kernel/vm_param.h>
#include <kernel/message.h>

#include <asm/context.h>


/* Thread states */
#define THREAD_RUNNING		0x01
#define THREAD_READY		0x02
#define THREAD_BLOCKED		0x10
#define THREAD_UNINT		(0x04 | THREAD_BLOCKED)	/* Thread on uninterruptable sleep */
#define THREAD_INT		(0x08 | THREAD_BLOCKED)	/* Thread on interruptable sleep */

#define SEND_BLOCKED		(0x20 | THREAD_BLOCKED | THREAD_UNINT)
#define RECEIVE_BLOCKED		(0x40 | THREAD_BLOCKED | THREAD_UNINT)
#define REPLY_BLOCKED		(0x80 | THREAD_BLOCKED | THREAD_UNINT)
#define IRQ_WAIT		(0x100| THREAD_BLOCKED | THREAD_UNINT)
#define SYNC_BLOCKED		(0x200| THREAD_BLOCKED | THREAD_UNINT)
#define THREAD_EXITING		0x400
#define PROCESS_EXITING		(0x800 | THREAD_EXITING)

/* No of reserved tids. */
#define FIRST_USER_TID		500

#ifndef __TID_T
#define __TID_T
 typedef int tid_t;		/* thread id. */
#endif


struct sched_param
{
  int sched_priority;
};

/*
 * The thread attributes as passed by user.
 */
struct thread_attr
{
  int flags;
  struct sched_param prio;
  void *stackaddr;
  size_t stack_size;
  int policy;
};

/*
 * the thread structure
 */
struct thread
{

  /* list of threads belonging to the same process. */
  struct linked_list thread_list;
#define threadnext thread_list.next
#define threadprev	thread_list.prev

  struct thread *next;		/* Next hash link */
  struct thread *waiting;	/* waiting for this thread to exit: (used by pthread_join()) */

  /* Linked list of ready to run threads. */
  struct linked_list runq;
  tid_t tid;			/* Thread id */

  /* Cpu related information of a thread. */
  struct context context;
  vm_offset_t stack;		/* kernel stack of the thread. */
  unsigned int states;		/* state of the thread. */
  struct process *proc;		/* Which process do i belong. */
  struct thread_attr attr;

#define schedpolicy 		attr.policy
#define priority_stat attr.prio.sched_priority

//      int policy;                     /* scheduling class for the thread. */
//      int priority_stat;              /* static priority. */
  int priority_dyn;		/* Dynamic priority */
  int bound;			/* Bound value to determine if it is CPU or I/O bound */
  int time_slice;
  int schedule;			/* schedule flag, if set, runs schedular when returning from kernel */
  int exitstat;

  /* Signal related */
  sigset_t sa_mask;
  sigset_t sigpending;

  /* ipc */
  struct message msg;		/* Copied Message */

  /* Queue of threads waiting to send a message to this thread. */
  struct linked_list rcvq;

  /* a linked list for a list of sending threads */
  struct linked_list sndq;
  int cpu;		/* Processor i belong to. */
#ifdef MANRIX_KPREEMPT
  int kpreempt;
#endif

};

typedef struct thread *thread_t;

/* Create a new thread */
extern thread_t create_thread (struct process *process,
			       struct thread_attr *attr);

/* Find a thread by its tid. */
extern thread_t find_by_tid (tid_t);
extern void sleep_interruptible (thread_t);
extern void sleep_uninterruptible (thread_t);
extern void thread_init ();

extern void exit_threads (list_t);

extern int destroy_thread (thread_t);
extern void change_tid (thread_t, tid_t);
extern int thread_wait (tid_t, int *);

extern void exec_thread (thread_t);

#endif /* __KERNEL_THREAD_H */
