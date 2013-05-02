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
 * File: sys/mman.h
 *  memory management declarations
 */

#ifndef __SYS_MMAN_H
#define __SYS_MMAN_H

#ifndef __SYS_TYPES_H
 #include <sys/types.h>
#endif	/* __SYS_TYPES_H */

/*  Memory protection options  */
#define PROT_READ 		1	/* Page can be read. */
#define PROT_WRITE 		2	/* Page can be written. */
#define PROT_EXEC 		4	/* Page can be executed. */
#define PROT_NONE 		8	/* Page cannot be accessed. */


/* flag options */
/* manrix only */
#define MAP_ZEROFILL		1	/* For Zero fill Memory */
#define MAP_ANYWHERE		2	/* The object in the entry can be mapped anywhere. */
/* manrix only */

#define MAP_FIXED		4	/* Interpret addr exactly. */
#define MAP_ANON		8	/* For anonymous Mapping */
#define MAP_SHARED		0x10	/* Share changes. */
#define MAP_PRIVATE		0x20	/* Changes are private. */


/* flags defined for msync(). */
#define MS_ASYNC			/* Perform asynchronous writes. */
#define MS_SYNC				/* Perform synchronous writes. */
#define MS_INVALIDATE			/* Invalidate mappings. */


/* symbolic constants defined for the mlockall() function. */
#define MCL_CURRENT			/* Lock currently mapped pages. */
#define MCL_FUTURE			/* Lock pages that become mapped. */

/*
 * mlock - lock a range of process address space (REALTIME)
 */
int    mlock(const void *, size_t);

/*
 * mlockall - lock/unlock the address space of a process
 */
int    mlockall(int);

/*
 * mmap - map pages of memory
 */
void  *mmap(void *, size_t, int, int, int, off_t);

/*
 * mprotect - set protection of memory mapping
 */
int    mprotect(void *, size_t, int);

int    msync(void *, size_t, int);

/*
 * munlock - unlock a range of process address space (REALTIME)
 */
int    munlock(const void *, size_t);

/*
 * munlockall - lock/unlock the address space of a process
 */
int    munlockall(void);

/*
 * munmap - unmap pages of memory
 */
int    munmap(void *, size_t);


#endif /* __SYS_MMAN_H */
