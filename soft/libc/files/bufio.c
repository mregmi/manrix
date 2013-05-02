/*
 * Libc for ManRix OS
 * Copyright (C) 2004, 2005	Manish Regmi (regmi dot manish at gmail dot com)
 *							Rajesh Bikram R.C   (rajee5283 at hotmail dot com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/*
 * File: bufio.c
 * 		Buffered I/O Handling.
 */

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/stat.h>
#include <asm/ipc.h>

FILE stdin_s = {0, NULL, 0, 0, 0};
FILE stdout_s = {1, NULL, 0, 0, 0};
FILE stderr_s = {2, NULL, 0, 0, 0};

FILE *stdin = &stdin_s;
FILE *stdout = &stdout_s;
FILE *stderr = &stderr_s;

/* Open a file stream on NAME.  */
FILE *
fopen (const char *name, const char *mode)
{
  FILE *filep;
  int fd, opmode = 0;
  size_t allocation;
//  struct stat statbuf;
  int save_errno;
  char *opt;

  if (name[0] == '\0')
    {
      /* POSIX.1-1990 says an empty name gets ENOENT;
	 but `open' might like it fine.  */
      __set_errno (ENOENT);
      return NULL;
    }
  if((opt = strchr(mode, 'r')) != NULL)
		opmode |= O_RDONLY;
  if((opt = strchr(mode, 'w')) != NULL)
		opmode |= O_WRONLY;
  if((opt = strchr(mode, '+')) != NULL)
		opmode |= O_APPEND;

  fd = open(name, opmode);
  if (fd < 0)
    return NULL;
  /*
  if(stat(name, &statbuf) < 0)
  	goto lose;

  if(!S_ISREG(statbuf.st_mode))
  	goto lose;
  */
  allocation = BUFSIZ;//statbuf.st_blksize;

  filep = (FILE *) malloc (sizeof (FILE) + allocation); /* Zero-fill.  */
  if (filep == NULL)
//  lose:
    {
     save_errno = errno;
      (void) close (fd);
      __set_errno (save_errno);
      return NULL;
    }
    memset(filep, 0, sizeof (FILE) + allocation);
  filep->data = (char *) (filep + 1);
  filep->allocation = allocation;
  filep->fd = fd;

  return filep;
}



/* Read a directory entry from DIRP.  */
size_t
fread (void *ptr, size_t size, size_t nitems, FILE *stream)
{
	int saved_errno = errno;
	size_t bytes;

	/* The Buffer is empty. refill it */
	if(stream->offset >= stream->size)
	{
		bytes = read(stream->fd, stream->data, stream->allocation);
		if (bytes <= 0)
		{
	      		/* On some systems getdents fails with ENOENT when the
		 	open directory has been rmdir'd already.  POSIX.1
		 	requires that we treat this condition like normal EOF.  */
	    	 	if (bytes < 0 && errno == ENOENT)
				bytes = 0;

	      		/* Don't modifiy errno when reaching EOF.  */
	      		if (bytes == 0)
	      		{
				__set_errno (saved_errno);
				return EOF;
			}
		}
	}

	return 0;
}


/* Close the file stream filep.
   Return 0 if successful, -1 if not.  */
int
fclose (FILE *filep)
{
  int fd;

  if (filep == NULL)
    {
      __set_errno (EINVAL);
      return -1;
    }

  /* We do not try to synchronize access here.  If some other thread
     still uses this handle it is a big mistake and that thread
     deserves all the bad data it gets.  */

  fd = filep->fd;

  free ((void *) filep);

  return close (fd);
}


/* Seek to position POS in filep.  */
int
fseek (filep, offset, whence)
     FILE *filep;
     long offset;
     int whence;
{
  (void) lseek (filep->fd, offset, whence);
  filep->size = 0;
  filep->offset = offset;
  //dirp->filepos = offset;
  
  return offset;
}


/* Return the current position of filep.  */
long
ftell (FILE *filep)
{
  return filep->offset;
}

/*
 * fgetc - get a byte from a stream
 */
int fgetc(FILE *stream)
{
	int saved_errno = errno;
	size_t bytes;
	char ch;

	if(!stream)
	{
		__set_errno(EBADF);
		return -1;
	}

	/* The Buffer is empty. refill it */
	if(stream->offset >= stream->size)
	{
		bytes = read(stream->fd, stream->data, stream->allocation);
		if (bytes <= 0)
		{
		      /* On some systems getdents fails with ENOENT when the
		 open directory has been rmdir'd already.  POSIX.1
		 requires that we treat this condition like normal EOF.  */
	    	 if (bytes < 0 && errno == ENOENT)
				bytes = 0;

	      /* Don't modifiy errno when reaching EOF.  */
	      	if (bytes == 0)
	      	{
				__set_errno (saved_errno);
				return EOF;
		}
		}
	 stream->size = (size_t) bytes;

	 /* Reset the offset into the buffer.  */
	 stream->offset = 0;
	}
	ch = stream->data[stream->offset];
	stream->offset++;
	
	return (int)ch;
}
