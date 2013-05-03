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
 * The header is as defined by POSIX 1003.1
 * Posix Threads declarations.
 */

#ifndef __PTHREAD_H
#define __PTHREAD_H

#ifndef __SYS_TYPES_H
#include <sys/types.h>
#endif	/* __SYS_TYPES_H */

#ifndef __SCHED_H
#include <sched.h>
#endif	/* __SCHED_H */

#ifndef __TIME_H
#include <time.h>
#endif	/* __TIME_H */

#ifndef PTHREAD_STACK_MIN
 #define PTHREAD_STACK_MIN			0x1000
#endif
 
#define PTHREAD_STACK_DEF			0x2000

#define PTHREAD_EXPLICIT_SCHED			0x0001
#define PTHREAD_INHERIT_SCHED			0x0002

#define PTHREAD_SCOPE_PROCESS			0x0004
#define PTHREAD_SCOPE_SYSTEM			0x0008

#define PTHREAD_CREATE_DETACHED			0x0010
#define PTHREAD_CREATE_JOINABLE			0x0020

#define PTHREAD_PRIO_INHERIT			1
#define PTHREAD_PRIO_NONE				2
#define PTHREAD_PRIO_PROTECT			3

#define PTHREAD_PROCESS_SHARED			1
#define PTHREAD_PROCESS_PRIVATE			2


#define PTHREAD_BARRIER_SERIAL_THREAD
#define PTHREAD_CANCEL_ASYNCHRONOUS
#define PTHREAD_CANCEL_ENABLE
#define PTHREAD_CANCEL_DEFERRED
#define PTHREAD_CANCEL_DISABLE
#define PTHREAD_CANCELED
#define PTHREAD_COND_INITIALIZER


#define PTHREAD_MUTEX_DEFAULT			MUTEX_DEFAULT
#define PTHREAD_MUTEX_ERRORCHECK		MUTEX_ERRORCHECK
#define PTHREAD_MUTEX_NORMAL			MUTEX_NORMAL
#define PTHREAD_MUTEX_RECURSIVE			MUTEX_RECURSIVE

#define PTHREAD_MUTEX_INITIALIZER		MUTEX_INIT_UNLOCKED



#define PTHREAD_ONCE_INIT

/*
 * pthread_atfork - register fork handlers
 */
int   pthread_atfork(void (*)(void), void (*)(void),
          void(*)(void));

/*
 * pthread_attr_destroy - destroy the thread attributes object
 */
int   pthread_attr_destroy(pthread_attr_t *);

/*
 * pthread_attr_getdetachstate - get and set the detachstate attribute
 */
int   pthread_attr_getdetachstate(const pthread_attr_t *, int *);

int   pthread_attr_getguardsize(const pthread_attr_t *__restrict,
          size_t *__restrict);

/*
 * pthread_attr_getinheritsched -
 * get the inheritsched attribute (REALTIME THREADS)
 */
int   pthread_attr_getinheritsched(const pthread_attr_t *__restrict,
          int *__restrict);

/*
 * pthread_attr_getschedparam - get and set the schedparam attribute
 */
int   pthread_attr_getschedparam(const pthread_attr_t *__restrict,
          struct sched_param *__restrict);

/*
 * pthread_attr_getschedpolicy -
 * get the schedpolicy attribute (REALTIME THREADS)
 */
int   pthread_attr_getschedpolicy(const pthread_attr_t *__restrict,
          int *__restrict);

/*
 * pthread_attr_getscope - get the contentionscope attribute (REALTIME THREADS)
 */
int   pthread_attr_getscope(const pthread_attr_t *__restrict,
          int *__restrict);

/*
 * pthread_attr_getstack - get stack attributes
 */
int   pthread_attr_getstack(const pthread_attr_t *__restrict,
          void **__restrict, size_t *__restrict);

/*
 * pthread_attr_getstackaddr - get the stackaddr attribute
 */
int   pthread_attr_getstackaddr(const pthread_attr_t *__restrict,
          void **__restrict);


int   pthread_attr_getstacksize(const pthread_attr_t *__restrict,
          size_t *__restrict);

/*
 * pthread_attr_init - destroy the thread attributes object
 */
int   pthread_attr_init(pthread_attr_t *);

/*
 * pthread_attr_getdetachstate - get the detachstate attribute
 */
int   pthread_attr_setdetachstate(pthread_attr_t *, int);

int   pthread_attr_setguardsize(pthread_attr_t *, size_t);

/*
 * pthread_attr_setinheritsched -
 * set the inheritsched attribute (REALTIME THREADS)
 */
int   pthread_attr_setinheritsched(pthread_attr_t *, int);

/*
 * pthread_attr_setschedparam - set the schedparam attribute
 */
int   pthread_attr_setschedparam(pthread_attr_t *__restrict,
          const struct sched_param *__restrict);
/*
 * pthread_attr_setschedpolicy -
 * set the schedpolicy attribute (REALTIME THREADS)
 */
int   pthread_attr_setschedpolicy(pthread_attr_t *, int);

/*
 * pthread_attr_setscope - set the contentionscope attribute (REALTIME THREADS)
 */
int   pthread_attr_setscope(pthread_attr_t *, int);

/*
 * pthread_attr_setstack - set stack attributes
 */
int   pthread_attr_setstack(pthread_attr_t *, void *, size_t);

/*
 * pthread_attr_setstackaddr - set the stackaddr attribute
 */
int   pthread_attr_setstackaddr(pthread_attr_t *, void *);

int   pthread_attr_setstacksize(pthread_attr_t *, size_t);


int   pthread_barrier_destroy(pthread_barrier_t *);
int   pthread_barrier_init(pthread_barrier_t *__restrict,
          const pthread_barrierattr_t *__restrict, unsigned);
int   pthread_barrier_wait(pthread_barrier_t *);
int   pthread_barrierattr_destroy(pthread_barrierattr_t *);


int   pthread_barrierattr_getpshared(
          const pthread_barrierattr_t *__restrict, int *__restrict);


int   pthread_barrierattr_init(pthread_barrierattr_t *);


int   pthread_barrierattr_setpshared(pthread_barrierattr_t *, int);

int   pthread_cancel(pthread_t);
void  pthread_cleanup_push(void (*)(void *), void *);
void  pthread_cleanup_pop(int);
int   pthread_cond_broadcast(pthread_cond_t *);
int   pthread_cond_destroy(pthread_cond_t *);
int   pthread_cond_init(pthread_cond_t *__restrict,
          const pthread_condattr_t *__restrict);
int   pthread_cond_signal(pthread_cond_t *);
int   pthread_cond_timedwait(pthread_cond_t *__restrict,
          pthread_mutex_t *__restrict, const struct timespec *__restrict);
int   pthread_cond_wait(pthread_cond_t *__restrict,
          pthread_mutex_t *__restrict);
int   pthread_condattr_destroy(pthread_condattr_t *);

int   pthread_condattr_getclock(const pthread_condattr_t *__restrict,
          clockid_t *__restrict);


int   pthread_condattr_getpshared(const pthread_condattr_t *__restrict,
          int *__restrict);

int   pthread_condattr_init(pthread_condattr_t *);

int   pthread_condattr_setclock(pthread_condattr_t *, clockid_t);


int   pthread_condattr_setpshared(pthread_condattr_t *, int);

/*
 * pthread_create - thread creation
 */
int   pthread_create(pthread_t *__restrict, const pthread_attr_t *__restrict,
          void *(*)(void *), void *__restrict);

int   pthread_detach(pthread_t);

/*
 * pthread_equal - compare thread IDs
 */
int   pthread_equal(pthread_t, pthread_t);

/*
 * pthread_exit - thread termination
 */
void  pthread_exit(void *);

int   pthread_getconcurrency(void);


int   pthread_getcpuclockid(pthread_t, clockid_t *);


int   pthread_getschedparam(pthread_t, int *__restrict,
          struct sched_param *__restrict);

void *pthread_getspecific(pthread_key_t);

/*
 * pthread_join - wait for thread termination
 */
int   pthread_join(pthread_t, void **);

int   pthread_key_create(pthread_key_t *, void (*)(void *));
int   pthread_key_delete(pthread_key_t);

/*
 * pthread_mutex_destroy - destroy a mutex
 */
int   pthread_mutex_destroy(pthread_mutex_t *);

/*
 * pthread_mutexattr_getprioceiling - get the prioceiling attribute
 * of the mutex attributes object (REALTIME THREADS)
 */
int   pthread_mutex_getprioceiling(const pthread_mutex_t *__restrict,
          int *__restrict);

/*
 * pthread_mutex_init - initialize a mutex
 */
int   pthread_mutex_init(pthread_mutex_t *__restrict,
          const pthread_mutexattr_t *__restrict);

/*
 * pthread_mutex_lock - lock a mutex
 */
int   pthread_mutex_lock(pthread_mutex_t *);

/*
 * pthread_mutexattr_setprioceiling - set the prioceiling attribute
 * of the mutex attributes object (REALTIME THREADS)
 */
int   pthread_mutex_setprioceiling(pthread_mutex_t *__restrict, int,
          int *__restrict);

/*
 * pthread_mutex_timedlock - lock a mutex (ADVANCED REALTIME)
 */
int   pthread_mutex_timedlock(pthread_mutex_t *,
          const struct timespec *);

/*
 * pthread_mutex_trylock - lock a mutex
 */
int   pthread_mutex_trylock(pthread_mutex_t *);

/*
 * pthread_mutex_unlock - unlock a mutex
 */
int   pthread_mutex_unlock(pthread_mutex_t *);

/*
 * pthread_mutexattr_destroy - destroy the mutex attributes object
 */
int   pthread_mutexattr_destroy(pthread_mutexattr_t *);

/*
 * pthread_mutexattr_getprioceiling - get the prioceiling attribute
 * of the mutex attributes object (REALTIME THREADS)
 */
int   pthread_mutexattr_getprioceiling(
          const pthread_mutexattr_t *__restrict, int *__restrict);

/*
 * pthread_mutexattr_getprotocol - get the protocol attribute
 * of the mutex attributes object (REALTIME THREADS)
 */
int   pthread_mutexattr_getprotocol(const pthread_mutexattr_t *__restrict,
          int *__restrict);

/*
 * pthread_mutexattr_getpshared - get the process-shared attribute
 */
int   pthread_mutexattr_getpshared(const pthread_mutexattr_t *__restrict,
          int *__restrict);

/*
 * pthread_mutexattr_gettype - get the mutex type attribute
 */
int   pthread_mutexattr_gettype(const pthread_mutexattr_t *__restrict,
          int *__restrict);

/*
 * pthread_mutexattr_init - initialize the mutex attributes object
 */
int   pthread_mutexattr_init(pthread_mutexattr_t *);

/*
 * pthread_mutexattr_setprioceiling - set the prioceiling attribute
 * of the mutex attributes object (REALTIME THREADS)
 */
int   pthread_mutexattr_setprioceiling(pthread_mutexattr_t *, int);

/*
 * pthread_mutexattr_setprotocol - set the protocol attribute
 * of the mutex attributes object (REALTIME THREADS)
 */
int   pthread_mutexattr_setprotocol(pthread_mutexattr_t *, int);

/*
 * pthread_mutexattr_setpshared - set the process-shared attribute
 */
int   pthread_mutexattr_setpshared(pthread_mutexattr_t *, int);

/*
 * pthread_mutexattr_settype - set the mutex type attribute
 */
int   pthread_mutexattr_settype(pthread_mutexattr_t *, int);

int   pthread_once(pthread_once_t *, void (*)(void));
int   pthread_rwlock_destroy(pthread_rwlock_t *);
int   pthread_rwlock_init(pthread_rwlock_t *__restrict,
          const pthread_rwlockattr_t *__restrict);
int   pthread_rwlock_rdlock(pthread_rwlock_t *);

int   pthread_rwlock_timedrdlock(pthread_rwlock_t *__restrict,
          const struct timespec *__restrict);
int   pthread_rwlock_timedwrlock(pthread_rwlock_t *__restrict,
          const struct timespec *__restrict);

int   pthread_rwlock_tryrdlock(pthread_rwlock_t *);
int   pthread_rwlock_trywrlock(pthread_rwlock_t *);
int   pthread_rwlock_unlock(pthread_rwlock_t *);
int   pthread_rwlock_wrlock(pthread_rwlock_t *);
int   pthread_rwlockattr_destroy(pthread_rwlockattr_t *);

int   pthread_rwlockattr_getpshared(
          const pthread_rwlockattr_t *__restrict, int *__restrict);

int   pthread_rwlockattr_init(pthread_rwlockattr_t *);

int   pthread_rwlockattr_setpshared(pthread_rwlockattr_t *, int);

/*
 * pthread_self - get the calling thread ID
 */
pthread_t pthread_self(void);

int   pthread_setcancelstate(int, int *);
int   pthread_setcanceltype(int, int *);

int   pthread_setconcurrency(int);


int   pthread_setschedparam(pthread_t, int,
          const struct sched_param *);


int   pthread_setschedprio(pthread_t, int);

int   pthread_setspecific(pthread_key_t, const void *);

/*
 * pthread_spin_destroy - destroy a spin lock object (ADVANCED REALTIME THREADS)
 */
int   pthread_spin_destroy(pthread_spinlock_t *);

/*
 * pthread_spin_init - initialize a spin lock object (ADVANCED REALTIME THREADS)
 */
int   pthread_spin_init(pthread_spinlock_t *, int);
int   pthread_spin_lock(pthread_spinlock_t *);
int   pthread_spin_trylock(pthread_spinlock_t *);
int   pthread_spin_unlock(pthread_spinlock_t *);

void  pthread_testcancel(void);



#endif /* __PTHREAD_H */

