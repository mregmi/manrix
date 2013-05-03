/*
 * stat for ManRix OS
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
 * File: stat.c
 * 		A utility to get file statistics
 */

#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static char *get_file_type(mode_t mode)
{
  char *type;
  
  if(S_ISDIR(mode))
  	type = "directory";
  else if(S_ISREG(mode))
  	type = "regular file";
  else if(S_ISLNK(mode))
  	type = "symbolic link";
  else if(S_ISBLK(mode))
  	type = "block special file";
  else if(S_ISCHR(mode))
  	type = "character special file";
  else if(S_ISSOCK(mode))
  	type = "socket special file";
  else if(S_ISFIFO(mode))
  	type = "FIFO special file";
  else
  	type = "unknown";

  return type;	
}


int main(int argc, char *argv[])
{
  int ret;
  struct stat st;
  char *path = "/bin/stat";
  
  ret = stat(path, &st);
  if(ret < 0)
  {
  	fprintf("Could not stat file \"%s\"", path);
	return 0;
  }
  
  printf("\n  File: '%s'", path);
  printf("\n  Size: %-15d Blocks: %-15d IO Block: %-8d" \
  	, st.st_size, st.st_blocks, st.st_blksize, get_file_type(st.st_mode));
  printf("\nDevice: %-15d Inode: %-15d Links: %-8d", st.st_dev, st.st_ino, st.st_nlink);
    
  return 0;
}
