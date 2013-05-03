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
 *
 *
 * File: ext2fs.c
 * 		EXT2 File System Directory related operations.
 */

#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <pthread.h>
#include <mrx/fd.h>

#include "vfs.h"
#include "ext2.h"

/* Check For execute Permission In the file */
static inline void can_execute(struct userid *id, struct ext2inode *ino)
{

}

/* Check For read Permission In the file */
static inline void can_read(struct userid *id, struct ext2inode *ino)
{

}

/* Check For write Permission In the file */
static inline void can_write(struct userid *id, struct ext2inode *ino)
{

}


/* Compare the name, wildcard support */
int check_name(char *dest, uint8_t nlen, char *src)
{
	int i=0,j=0, srclen;

	srclen = strlen(src);
	for(i=0,j=0;i<srclen;i++,j++)
	{

		if(src[i] == '?')
			continue;

		if(src[i] == '*')
		{
			for(;src[i] != '.';i++)
				if(src[i] == '\0')
					return 0;
			for(;dest[j] != '.';j++)
				if(src[j] == '\0')
					return 0;
		}

		if(src[i] != dest[j])
			return 1;
	}

	/* They are not equal */
	if(i != nlen)
		return 1;

	return 0;
}

/* Get the blocks used by the file */
static inline uint32_t get_file_blk(struct vnode *vn)
{
	struct ext2inode *in = &vn->v_ino.ext2inode;

	return (in->i_blocks * 512);
}

/* Get the Link for the symlink inode Number */
static inline char *follow_symlink(struct vnode *parent, uint32_t inum)
{
	char *path;
	struct vnode *vlink;

	vlink = vget(parent, inum, parent->v_rdev);
	path = (char *)vlink->v_ino.ext2inode.i_block;

	return path;
}

/* Read the path and return the vnode */
int ext2_readdir(struct vnode *parent, char *path, struct nameidata *nm)
{
	uint32_t block_no, blkindex = 0, blksz;
	struct ext2inode *eino = &parent->v_ino.ext2inode;
	struct bcache *entry;
	struct ext2dirent *dir;
	struct vnode *vnode = parent;
	uint32_t last_sym= 0, entrysz;
	char name[EXT2_NAME_LEN];
	char *pth = path;
	int ret= -EINVAL , comp = 0, index = 0;

//	printf("reading dir %d ",sizeof(struct ext2dirent));
again:
	do{
		while(*pth == '/')
			pth++;

		while (*pth != '/')
		{
			if(*pth == '\0')
				break;
			name[index] = *pth++;
			index++;
		}
		name[index] = 0;
		if((*pth == '\0') || (*(pth + 1) == '\0'))
			comp = 1;

		pthread_mutex_lock(&vnode->smlock);
		do{
			if(!(vnode->v_type & VNODE_DIR))
			{
				ret = -EINVAL;

				goto err;
			}

			/* Folder found but it is a mountpoint */
			if(vnode->v_fsmounted)
			{
				nm->vfs = vnode->v_fsmounted;
				return 1;
			}

			can_execute(&nm->id, eino);
			block_no = ext2_bmap(vnode, blkindex);

			entry = (struct bcache *)bread(vnode->v_rdev, block_no, vnode->vfsp->blk_size);
		//	printf("reading block %d dev %x data %d", block_no, vnode->v_rdev, entry->b_data);

			if(!entry)
				return -EINTR;

			dir = (struct ext2dirent *)entry->b_data;
			entrysz = ((uint32_t)entry->b_data + entry->b_datasz);
			while((dir->rec_len != 0))
			{
				if(dir->namelen > EXT2_NAME_LEN)
					return (-ENAMETOOLONG);

/*				if(FT_ISLINK(dir->file_type))
				{
					if(nm->access & O_SYMLINK)
						goto fileok;
					if(last_sym == dir->inum)
						return -ELOOP;

					pth = follow_symlink(vnode, dir->inum);
					index = 0;

					if(*pth == '/')
						vnode = get_root_vnode();

					goto again;
				}
*/
				ret = check_name(dir->name, dir->namelen, name);
//				printf(" %s ", dir->name);

				if(ret == 0)
					goto fileok;
				dir = (struct ext2dirent *)(((uint32_t)dir)+dir->rec_len);

				if((uint32_t)dir >= entrysz)
					break;
			}

			brelse(entry);
			/* File does not exist */
		//	if((uint32_t)dir < entrysz)
		//		break;
			
			blksz = get_file_blk(vnode);
			blkindex++;
//			printf("blksz %d bindex %d", blksz, blkindex);
			if((blkindex * vnode->vfsp->blk_size)	>= blksz)
				break;
	
		}while(1);

		/* Last entry. Create if it is a file */
		if(comp == 1)
			nm->vn = vnode;
		pthread_mutex_unlock(&vnode->smlock);
		return -1;

		/* File Found */
	fileok:
		pthread_mutex_unlock(&vnode->smlock);
		vnode = vget(vnode, dir->inum, vnode->v_rdev);

		if(vnode == NULL)
			return -1;
		eino = &vnode->v_ino.ext2inode;
//		printf("going again %d ", vnode->v_inum);
//		sched_yield();
		/* Check for folder perm. false = -EACCESS */
		can_execute(&nm->id, eino);
		//if(vnext->)


		blkindex = 0;
		index = 0;
	}while(comp == 0);

	nm->vn = vnode;
	pthread_mutex_lock(&vnode->smlock);
	return 0;
err:
	pthread_mutex_unlock(&vnode->smlock);
	return ret;
}

