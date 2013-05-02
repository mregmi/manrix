/*
 * FS and I/O manager for XXX OS
 * Copyright (C) 2004, 2005	Manish Regmi (regmi dot manish at gmail dot com)
 *				Rajesh R.C (rajee5283 at hotmail dot com)
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
 * File: vfs_namei.c
 * 		The name to vnode Operations independent of File System.
 */

#include <pthread.h>

#include "vfs.h"

#include <asm/syscall.h>

/* Pointer to the vfs of root */
extern struct vfs *rootvfs;


/*
 * File System independent Name to vnode
 * the access value returns error code.
 */
struct vnode *vfs_namev(tid_t tid, char *path, int *access, mode_t mode)
{
	struct vnode *cwd;
	struct nameidata ndata;
	char *name = path;
	int status = 0;
	pid_t pid;

	if(path[0] == '/')
	{
		cwd = get_root_vnode();
		if(path[1] == '\0')
		{
			printf("Root file");
			return cwd;
		}
	}
	else
		cwd = get_cwd(tid);
		
	if(!cwd->ops || !cwd->ops->vop_namei)
		return NULL;

//	printf("WOW XX");	
	ndata.mode = mode;
	ndata.access = *access;
	pid = syscall_get_pid(tid);
//	printf("tid %d pid = %d ", tid, pid);
//	for(;;);
	syscall_get_user(pid, &ndata.id);

again:
	status = cwd->ops->vop_namei(cwd, name, &ndata);
//	printf("hellooppo %d ",status);
	sched_yield();
//	for(;;);
	/*
	 * status < 0 = ERROR
	 * status = 0 = SUCCESS
	 * status > 0 = Mount Point reached
	 */
	if(status > 0)
	{
		name = ndata.path;
		cwd = ndata.vfs->vfs_mpoint;
		pthread_mutex_unlock(&ndata.vn->smlock);
		goto again;
	}
	if(status < 0)
	{
		*access = status;
		return NULL;
	}

//	*access = 0; /* No error code */

	return ndata.vn;
}
