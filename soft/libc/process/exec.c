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
 * File:- exec.c
 */

/* Copyright (C) 1991,97,98,99,2002 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.
*/

#include <config.h>

#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>

#include <mrx/pager.h>
#include <mrx/fd.h>
#include <mrx/conmngr.h>
#include <asm/ipc.h>
#include <asm/syscall.h>


/*
 * execv - execute a file
 */
int __execve(const char *path, char *const argv[], char *const envp[])
{
	int ret, length;
	struct vn_object obj;
	struct message msg;

	length = strlen(path);
	if(length > MAX_MSG_LONG)
		msg.flags = MSG_MAP_SEND;
	else
		msg.flags = MSG_LONG_SEND;

	msg.flags |= MSG_LONG_RECEIVE;
	msg.snd_ptr = (void *)path;
	msg.snd_bufsize = length+1;
	msg.rcv_ptr = &obj;
	msg.rcv_bufsize = sizeof(struct vn_object);
	msg.w1 = FS_EXEC;
	send_message(ID_FSIO_MNGR, &msg);

	ret = msg.w1;
	if(ret < 0)
	{
		errno = -ret;
		return -1;
	}

	syscall_exec((void *)obj.va, &obj, (void *)argv, (void *)envp);

	return ret;
}

/* Execute PATH with arguments ARGV and environment from `environ'.  */
int
execv (const char *path, char *const argv[])
{
	return __execve(path, argv, environ);
}


/* Execute FILE, searching in the `PATH' environment variable if it contains
   no slashes, with arguments ARGV and environment from `environ'.  */
int
execvp (file, argv)
     const char *file;
     char *const argv[];
{
  if (*file == '\0')
    {
      /* We check the simple case first. */
      __set_errno (ENOENT);
      return -1;
    }

  if (strchr (file, '/') != NULL)
    {
      /* Don't search when it contains a slash.  */
      __execve (file, argv, __environ);

//      if (errno == ENOEXEC)
//	script_execute (file, argv);
    }
  else
    {
      int got_eacces = 0;
      char *path, *p, *name;
      size_t len;
      size_t pathlen;

      path = getenv ("PATH");
      if (path == NULL)
	{
	  /* There is no `PATH' in the environment.
	     The default search path is the current directory
	     followed by the path `confstr' returns for `_CS_PATH'.  */
	  len = confstr (_CS_PATH, (char *) NULL, 0);
	  path = (char *) malloc (1 + len);
	  path[0] = ':';
	  (void) confstr (_CS_PATH, path + 1, len);
	}

      len = strlen (file) + 1;
      pathlen = strlen (path);
      name = malloc (pathlen + len + 1);
      /* Copy the file name at the top.  */
      name = (char *) memcpy (name + pathlen + 1, file, len);
      /* And add the slash.  */
      *--name = '/';

      p = path;
      do
	{
	  char *startp;

	  path = p;
	  p = __strchrnul (path, ':');

	  if (p == path)
	    /* Two adjacent colons, or a colon at the beginning or the end
	       of `PATH' means to search the current directory.  */
	    startp = name + 1;
	  else
	    startp = (char *) memcpy (name - (p - path), path, p - path);

	  /* Try to execute this name.  If it works, execv will not return.  */
	  __execve (startp, argv, __environ);

//	  if (errno == ENOEXEC)
//	    script_execute (startp, argv);

	  switch (errno)
	    {
	    case EACCES:
	      /* Record the we got a `Permission denied' error.  If we end
		 up finding no executable we can use, we want to diagnose
		 that we did find one but were denied access.  */
	      got_eacces = 1;
	    case ENOENT:
	    case ESTALE:
	    case ENOTDIR:
	      /* Those errors indicate the file is missing or not executable
		 by us, in which case we want to just try the next path
		 directory.  */
	      break;

	    default:
	      /* Some other error means we found an executable file, but
		 something went wrong executing it; return the error to our
		 caller.  */
	      return -1;
	    }
	}
      while (*p++ != '\0');

      /* We tried every element and none of them worked.  */
      if (got_eacces)
	/* At least one failure was due to permissions, so report that
           error.  */
	__set_errno (EACCES);
    }

  /* Return the error from the last attempt (probably ENOENT).  */
  return -1;
}

/* Execute FILE, searching in the `PATH' environment variable if
   it contains no slashes, with all arguments after FILE until a
   NULL pointer and environment from `environ'.  */
int
execlp (const char *file, const char *arg, ...)
{
  size_t argv_max = 1024;
  const char **argv = malloc (argv_max * sizeof (const char *));
  unsigned int i;
  va_list args;

  argv[0] = arg;

  va_start (args, arg);
  i = 0;
  while (argv[i++] != NULL)
    {
      if (i == argv_max)
	{
	  const char **nptr = malloc ((argv_max *= 2) * sizeof (const char *));

#ifndef _STACK_GROWS_UP
	  if ((char *) nptr + argv_max == (char *) argv)
	    {
	      /* Stack grows down.  */
	      argv = (const char **) memcpy (nptr, argv,
					     i * sizeof (const char *));
	      argv_max += i;
	    }
	  else
#endif
#ifndef _STACK_GROWS_DOWN
	    if ((char *) argv + i == (char *) nptr)
	    /* Stack grows up.  */
	    argv_max += i;
	  else
#endif
	    /* We have a hole in the stack.  */
	    argv = (const char **) memcpy (nptr, argv,
					   i * sizeof (const char *));
	}

      argv[i] = va_arg (args, const char *);
    }
  va_end (args);

  return execvp (file, (char *const *) argv);
}


/* Execute PATH with all arguments after PATH until a NULL pointer,
   and the argument after that for environment.  */
int
execle (const char *path, const char *arg, ...)
{
  size_t argv_max = 1024;
  const char **argv = malloc (argv_max * sizeof (const char *));
  const char *const *envp;
  unsigned int i;
  va_list args;
  argv[0] = arg;

  va_start (args, arg);
  i = 0;
  while (argv[i++] != NULL)
    {
      if (i == argv_max)
	{
	  const char **nptr = malloc ((argv_max *= 2) * sizeof (const char *));

#ifndef _STACK_GROWS_UP
	  if ((char *) nptr + argv_max == (char *) argv)
	    {
	      /* Stack grows down.  */
	      argv = (const char **) memcpy (nptr, argv,
					     i * sizeof (const char *));
	      argv_max += i;
	    }
	  else
#endif
#ifndef _STACK_GROWS_DOWN
	    if ((char *) argv + i == (char *) nptr)
	    /* Stack grows up.  */
	    argv_max += i;
	  else
#endif
	    /* We have a hole in the stack.  */
	    argv = (const char **) memcpy (nptr, argv,
					   i * sizeof (const char *));
	}

      argv[i] = va_arg (args, const char *);
    }

  envp = va_arg (args, const char *const *);
  va_end (args);

  return __execve (path, (char *const *) argv, (char *const *) envp);
}



/* Execute PATH with all arguments after PATH until
   a NULL pointer and environment from `environ'.  */
int
execl (const char *path, const char *arg, ...)
{
  size_t argv_max = 1024;
  const char **argv = malloc (argv_max * sizeof (const char *));
  unsigned int i;
  va_list args;

  argv[0] = arg;

  va_start (args, arg);
  i = 0;
  while (argv[i++] != NULL)
    {
      if (i == argv_max)
	{
	  const char **nptr = malloc ((argv_max *= 2) * sizeof (const char *));

#ifndef _STACK_GROWS_UP
	  if ((char *) nptr + argv_max == (char *) argv)
	    {
	      /* Stack grows down.  */
	      argv = (const char **) memcpy (nptr, argv,
					     i * sizeof (const char *));
	      argv_max += i;
	    }
	  else
#endif
#ifndef _STACK_GROWS_DOWN
	    if ((char *) argv + i == (char *) nptr)
	    /* Stack grows up.  */
	    argv_max += i;
	  else
#endif
	    /* We have a hole in the stack.  */
	    argv = (const char **) memcpy (nptr, argv,
					   i * sizeof (const char *));
	}

      argv[i] = va_arg (args, const char *);
    }
  va_end (args);

  return __execve (path, (char *const *) argv, __environ);
}

