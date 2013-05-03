/*
 * ManRiX Microkernel
 * Copyright (C) 2004, 2005
 * 	Manish Regmi (regmi dot manish at gmail dot com) and
 * 	Rajesh R.C   (rajee5283@hotmail.com)
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
 * File: fd.c
 * 		The file descriptor implementation.
 */

#include <string.h>

#include <kernel/scheduling.h>
#include <kernel/vm.h>

/* Open file table cache */
kmem_cache_t ftable_cache = NULL;

/*
 * Fork the open File Table
 */
int fd_fork(process_t chld, process_t parent)
{
	vm_offset_t *ptable, *ctable;
	struct file *pfile, *cfile;
	int index, max = parent->fd.fd_max - MAX_DEF_FD;

	ptable = (vm_offset_t *)&parent->fd.fileptr;
	ctable = (vm_offset_t *)&chld->fd.fileptr;

	for(index = 0; index < MAX_DEF_FD; index++)
	{
		if(ptable[index] == 0)
			continue;

		pfile = (struct file *)ptable[index];
		cfile = (struct file *)kmem_cache_alloc(ftable_cache, 0);
		if(!cfile)
			return -ENOMEM;

		memcpy(cfile, pfile, sizeof(struct file));
	}

	if(ptable[MAX_DEF_FD] == 0)
		return 0;

	ptable = (vm_offset_t *)ptable[MAX_DEF_FD];
	for(index = 0; index < max; index++)
	{
		if(ptable[index] == 0)
			continue;

		pfile = (struct file *)ptable[index];
		cfile = (struct file *)kmem_cache_alloc(ftable_cache, 0);
		if(!cfile)
			return -ENOMEM;

		memcpy(cfile, pfile, sizeof(struct file));
	}
	return 0;
}

/* Free the file table */
void fd_free(struct fd *fd)
{

	vm_offset_t *table = (vm_offset_t *)&fd->fileptr, ptr;
	int index, max = fd->fd_max - MAX_DEF_FD;

	for(index = 0; index < MAX_DEF_FD; index++)
	{
		ptr = table[index];
		if(ptr == 0)
			continue;

		kmem_cache_free(ftable_cache, (void *)ptr);
	}

	table = (vm_offset_t *)table[MAX_DEF_FD];
	for(index = 0; index < max; index++)
	{
		ptr = table[index];
		if(ptr == 0)
			continue;

		kmem_cache_free(ftable_cache, (void *)ptr);
	}

	kmem_free((vm_offset_t)table, PAGE_SIZE);
}

/*
 * Get a free descriptor from the descriptor table.
 * The max no of fd by defalult is MAX_DEF_FD(32).
 * But it grow on user request. Bitmap is used to
 * search for fd less than MAX_DEF_FD. For FD > 32 hint
 * based search is used.
 */
int alloc_fdesc(struct fd *fd)
{
	int desc, i;
	vm_offset_t *table = (vm_offset_t *)&fd->fileptr, t1;
	struct file *newfd;

	desc = ffc(fd->bitmap);

	/* We found the lowest valued fd; */
	if((desc < MAX_DEF_FD) && (desc >= 0))
	{
		set_bit(&fd->bitmap, desc);
		newfd = kmem_cache_alloc(ftable_cache, 0);
		table[desc] = (vm_offset_t)newfd;
		return desc;
	}

	/*
	 * No empty slot availabe in first 32 and
	 * this process cannot have more than MAX_DEF_FD.
	 */
	if(fd->fd_max <= MAX_DEF_FD)
		return -1;

	table = (vm_offset_t *)table[MAX_DEF_FD];
	if(!table)
		return -1;
	/*
	 * Check if the hint points to the free table
	 */
	if(table[fd->fd_hint] == 0)
	{
		desc = fd->fd_hint;
		newfd = kmem_cache_alloc(ftable_cache, 0);
		table[desc] = (vm_offset_t)newfd;
		fd->fd_hint++;

		return desc + MAX_DEF_FD;
	}

	/* Search the whole array */
	for(i = 0; i < fd->fd_max; i++)
	{
		t1 = table[i];
		if(t1 == 0)
		{
			fd->fd_hint = i+1;
			newfd = kmem_cache_alloc(ftable_cache, 0);
			table[i] = (vm_offset_t)newfd;

			return i + MAX_DEF_FD;
		}
	}

	return -1;
}

/* Free a descriptor */
int free_fdesc(struct fd *fd, int desc)
{
	vm_offset_t *table = (vm_offset_t *)&fd->fileptr, ptr;

	if(desc < 0 || desc > fd->fd_max)
		return -1;


	fd->fd_hint = desc;

	if(desc < MAX_DEF_FD)
	{
		clear_bit(&fd->bitmap, desc);
		ptr = table[desc];
		kmem_cache_free(ftable_cache, (void *)ptr);

		return 0;
	}

	table = (vm_offset_t *)table[MAX_DEF_FD];
	ptr = table[desc - MAX_DEF_FD];
	kmem_cache_free(ftable_cache, (void *)ptr);

	return 0;
}

/* Grow the File Descriptor table on user request */
int grow_fd(struct fd *fd, int fdmax)
{
//	struct fd *newfd;
	vm_offset_t *table = (vm_offset_t *)&fd->fileptr, ptr;
	size_t size = PAGE_SIZE;

	/* Do not decrease */
	if(fdmax <= MAX_DEF_FD)
		return 0;

	/* Now we create new file table pointer array in new page */

	ptr = (vm_offset_t)kmem_alloc(size);
	if(!ptr)
		return -ENOMEM;

	memset((void *)ptr, 0 , size);
	table[MAX_DEF_FD] = ptr;

	fd->fd_max = fdmax;

	return 0;
}


/* initialize fd */
void fd_init()
{
	ftable_cache = kmem_cache_create("ftable_cache",
		sizeof(struct file), 32, NULL, NULL);
}


/*
 * Duplicate the file descriptor
 */
int syscall_dup(void *rptr, int fd1, int fd2)
{
	struct fd *fd = &current->proc->fd;
	vm_offset_t *ptr, *table = NULL, t1;
	int desc, i;

	if((fd1 < 0) || (fd1 > fd->fd_max))
		return -EBADF;

	if((fd2 < 0) || (fd2 > fd->fd_max))
		return -EBADF;

	if(fd1 < MAX_DEF_FD)
		ptr = (vm_offset_t *)fd->fileptr[fd1];
	else
	{
		ptr = (vm_offset_t *)fd->fileptr[fd1];
		ptr = (vm_offset_t *)ptr[fd1 - MAX_DEF_FD];
	}

	if(ptr == NULL)
		return -EBADF;

	desc = ffc(fd->bitmap);

	/* We found the lowest valued fd; */
	if((desc < MAX_DEF_FD) && (desc >= 0))
	{
		set_bit(&fd->bitmap, desc);
		fd->fileptr[desc] = (vm_offset_t)ptr;
		return desc;
	}

	/*
	 * No empty slot availabe in first 32 and
	 * this process cannot have more than MAX_DEF_FD.
	 */
	if(fd->fd_max <= MAX_DEF_FD)
		return -1;

	table = (vm_offset_t *)table[MAX_DEF_FD];
	if(!table)
		return -1;
	/*
	 * Check if the hint points to the free table
	 */
	if(table[fd->fd_hint] == 0)
	{
		desc = fd->fd_hint;
		table[desc] = (vm_offset_t)ptr;
		fd->fd_hint++;

		return desc + MAX_DEF_FD;
	}

	/* Search the whole array */
	for(i = 0; i < fd->fd_max; i++)
	{
		t1 = table[i];
		if(t1 == 0)
		{
			fd->fd_hint = i+1;
			table[i] = (vm_offset_t)ptr;

			return i + MAX_DEF_FD;
		}
	}

	return 0;
}
