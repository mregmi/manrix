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
 * File: dirent.c
 * 		Directory Handling.
 */

#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <asm/ipc.h>

#define EXTRA_FLAGS	O_DIRECTORY

/* Open a directory stream on NAME.  */
DIR *
opendir (const char *name)
{
  DIR *dirp;
  int fd;
  struct stat statbuf;
  size_t allocation;
  int save_errno;

  if (name[0] == '\0')
    {
      /* POSIX.1-1990 says an empty name gets ENOENT;
	 but `open' might like it fine.  */
      __set_errno (ENOENT);
      return NULL;
    }

  fd = open(name, O_RDONLY|O_NDELAY|EXTRA_FLAGS);
  if (fd < 0)
    return NULL;

//  if(stat(name, &statbuf) < 0)
//  	goto lose;

//  if(!S_ISDIR(statbuf.st_mode))
//  	goto lose;

  allocation = 1024;//statbuf.st_blksize;

  dirp = (DIR *) calloc (1, sizeof (DIR) + allocation); /* Zero-fill.  */
  if (dirp == NULL)
  lose:
    {
      save_errno = errno;
      (void) close (fd);
      __set_errno (save_errno);
      return NULL;
    }
  dirp->data = (char *) (dirp + 1);
  dirp->allocation = allocation;
  dirp->fd = fd;

  return dirp;
}

/* Get more directory entries */
ssize_t
__getdirentries (fd, buf, nbytes)
     int fd;
     char *buf;
     size_t nbytes;
{
   return (ssize_t) read (fd, buf, nbytes);
}

/* Read a directory entry from DIRP.  */
struct dirent *
readdir (DIR *dirp)
{
  struct dirent *dp;
  int saved_errno = errno;


  do
    {
      size_t reclen;

      if (dirp->offset >= dirp->size)
	{
	  /* We've emptied out our buffer.  Refill it.  */

	  size_t maxread;
	  ssize_t bytes;

#ifndef _DIRENT_HAVE_D_RECLEN
	  /* Fixed-size struct; must read one at a time (see below).  */
	  maxread = sizeof *dp;
#else
	  maxread = dirp->allocation;
#endif

	  bytes = __getdirentries(dirp->fd, dirp->data, maxread);
	  if (bytes <= 0)
	    {
	      /* On some systems getdents fails with ENOENT when the
		 open directory has been rmdir'd already.  POSIX.1
		 requires that we treat this condition like normal EOF.  */
	      if (bytes < 0 && errno == ENOENT)
		bytes = 0;

	      /* Don't modifiy errno when reaching EOF.  */
	      if (bytes == 0)
		__set_errno (saved_errno);
	      dp = NULL;
	      break;
	    }
	  dirp->size = (size_t) bytes;

	  /* Reset the offset into the buffer.  */
	  dirp->offset = 0;
	}

      dp = (struct dirent *) &dirp->data[dirp->offset];

#ifdef _DIRENT_HAVE_D_RECLEN
      reclen = dp->d_reclen;
      dp->d_name[dp->d_namelen] = '\0';
#else
      /* The only version of `struct dirent*' that lacks `d_reclen'
	 is fixed-size.  */
      assert (sizeof dp->d_name > 1);
      reclen = sizeof *dp;
      /* The name is not terminated if it is the largest possible size.
	 Clobber the following byte to ensure proper null termination.  We
	 read jst one entry at a time above so we know that byte will not
	 be used later.  */
      dp->d_name[sizeof dp->d_name] = '\0';
#endif

      dirp->offset += reclen;

#ifdef _DIRENT_HAVE_D_OFF
      dirp->filepos = dp->d_off;
#else
      dirp->filepos += reclen;
#endif

      /* Skip deleted files.  */
    } while (dp->d_ino == 0);


  return dp;
}


/* Rewind DIRP to the beginning of the directory.  */

void
rewinddir (dirp)
     DIR *dirp;
{
  (void) lseek (dirp->fd, (off_t) 0, SEEK_SET);
  dirp->offset = 0;
  dirp->size = 0;
}


/* Close the directory stream DIRP.
   Return 0 if successful, -1 if not.  */
int
closedir (DIR *dirp)
{
  int fd;

  if (dirp == NULL)
    {
      __set_errno (EINVAL);
      return -1;
    }

  /* We do not try to synchronize access here.  If some other thread
     still uses this handle it is a big mistake and that thread
     deserves all the bad data it gets.  */

  fd = dirp->fd;

  free ((void *) dirp);

  return close (fd);
}


/* Seek to position POS in DIRP.  */
void
seekdir (dirp, pos)
     DIR *dirp;
     long int pos;
{
  (void) lseek (dirp->fd, pos, SEEK_SET);
  dirp->size = 0;
  dirp->offset = 0;
  dirp->filepos = pos;
}


/* Return the current position of DIRP.  */
long int
telldir (DIR *dirp)
{
  return dirp->filepos;
}
