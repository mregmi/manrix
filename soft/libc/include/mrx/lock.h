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
/*	file: lock.h
 *		Various types of lock for Synchronization.
 */


#ifndef __KERNEL_LOCK_H
#define __KERNEL_LOCK_H


#include <mrx/spinlock.h>
#include <mrx/barrier.h>
#include <mrx/condvar.h>
#include <mrx/mutex.h>
#include <mrx/rwlock.h>

#define SYNC_MUTEX	1
#define SYNC_CONDVAR	2

#endif /* __KERNEL_LOCK_H */
