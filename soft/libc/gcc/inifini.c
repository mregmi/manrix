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
 * File: inifini.c
 * 	The constructor and destructor for User Programs
 * The ManRiX_init is the constructor and ManRiX_fini is
 * the destructor for all user level applications.
 */

#include <string.h>
#include <mrx/pthread-internal.h>

extern char **environ;

/*
 * This Function is called before main.
 */
void ManRiX_init(char *user_data)
{
	struct pthread_internal pint;

	pint.pt_fork.prepare = NULL;
	pint.pt_fork.parent = NULL;
	pint.pt_fork.child = NULL;
//	pint.tid = pthread_self();

	/* pthread structure is after argc, argv pointer and envp pointer */
	memcpy((user_data + 12), (char *)&pint, sizeof(struct pthread_internal));

	/* Environment pointer */
	environ = NULL;//(char **)(*((unsigned long *)user_data + 2));

	ptmalloc_init();	/* ptmalloc initialization */

}

void ManRiX_fini(int ecode)
{
	exit(ecode);

	/* This will not return */
}
