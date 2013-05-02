/*
 * FS and I/O manager for XXX OS
 * Copyright (C) 2004, 2005	Manish Regmi (regmi dot manish at gmail dot com)
 *							Rajesh R.C (rajee5283 at hotmail dot com)
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
 * 	File: ext2inode.c
 * 		EXT2 File System Inode related operations.
 */

#include <string.h>

#include "vfs.h"
#include "ext2.h"
#include "bcache.h"

extern struct vnodeops ext2_vnode_ops;

/*struct vnode *get_inode(struct vnode *parent, ino_t inum)
{
	struct vnode *vnode;
	vnode = vget(parent, inum, parent->v_rdev);
	if(!vnode)
		return NULL;
	return vnode;

}
*/

static inline struct ext2_group_desc* group_desc_read
					(struct vnode* parent, unsigned int group)
{
	struct ext2_group_desc *gd;

	gd = &parent->vfsp->sb.ext2gdesc[group];

	return gd;
}

static inline int read_inode_index
	(struct vnode *parent ,struct ext2_group_desc* group_desc , \
	 unsigned long index, struct ext2inode* ino)
{
	struct bcache *bcache;
	struct ext2inode *inode;
	unsigned long bindex, block;

	bindex = index % EXT2_BLOCK_SIZE(&parent->vfsp->sb.ext2sb);
	block =  group_desc->bg_inode_table + (index/EXT2_BLOCK_SIZE(&parent->vfsp->sb.ext2sb));

	bcache = (struct bcache *)bread(parent->v_rdev, block, parent->vfsp->blk_size);
	if(!bcache)
		return -1;
	inode = (struct ext2inode *)(((unsigned long)bcache->b_data) + bindex);

//	printf("reading done data %x %d imode %x, isize %d \n", bcache->b_data, bindex ,inode->i_mode, inode->i_size);
	memcpy(ino, inode, sizeof(struct ext2inode));

	brelse(bcache);

	return 0;
}

/*Reading the inode */
int ext2_read_inode(struct vnode *parent, ino_t inum)
{
	unsigned long group;
	unsigned long index;
	struct ext2_group_desc *group_desc;
	struct ext2inode *inode = &parent->v_ino.ext2inode;

	group = (inum-1) / EXT2_INODES_PER_GROUP(&parent->vfsp->sb.ext2sb) ;


	/* Read the group descriptor using the group value */
	group_desc = group_desc_read(parent, group);

	index = ((inum-1) %
	 EXT2_INODES_PER_GROUP(&parent->vfsp->sb.ext2sb)) * sizeof(struct ext2inode);

//	 printf("reading group %d index %d size %d\n", group, index, sizeof(struct ext2_group_desc));
//	printf("\nReading %d %d", group_desc->bg_inode_table, parent->vfsp->sb.ext2gdesc->bg_inode_table);

	/* Read the inode of given index from inode table */
	read_inode_index(parent, group_desc, index, inode);
//	for(;;);
	parent->v_type = inode->i_mode	 /* & EXT2_S_IFMT */;
	parent->ops = (struct vnodeops *)&ext2_vnode_ops;
//	printf("ops %d ", parent->ops);

	return 0;
}

/* block mapping of logical file  */

unsigned long ext2_bmap( struct vnode *vnode, unsigned long offset)
{
	unsigned int blkno , itotal , tot, totalb;
	unsigned long *blk, blk_no;
	struct ext2inode *inode;
	struct bcache *bcache;

	inode = &vnode->v_ino.ext2inode;

	/* 0-12 offset is used for direct block mapping */
	if(offset < EXT2_IND_BLOCK)
	{
		blkno = inode->i_block[offset];
		return blkno;
	}
     /* Gives the total data block */
	itotal = vnode->vfsp->blk_size/sizeof(unsigned int);

	tot = itotal + EXT2_IND_BLOCK;
	/* offset 13 is used for single indirect block mapping */
	if(offset >= EXT2_IND_BLOCK && offset < tot)
	{
		blkno = inode->i_block[EXT2_IND_BLOCK];
		bcache = bread(vnode->v_rdev, blkno, vnode->vfsp->blk_size);
		blk = (unsigned long *)bcache->b_data;
/* FIXME */	blk_no = blk[offset - EXT2_IND_BLOCK];
   		brelse(bcache);
		
		return blk_no;
	}

	/* offset 14 is used for double direct block mapping */



	if( offset >= tot && offset <= (offset + tot))
	{
		blkno = inode->i_block[EXT2_DIND_BLOCK];
		/*reading double indirect block mapping */
		bcache = bread(vnode->v_rdev,blkno,vnode->vfsp->blk_size);
		blk = (unsigned long *)bcache->b_data;
		blk_no =blk[offset -tot];
		brelse(bcache);

		/* reading single indirect block */
		bcache = bread(vnode->v_rdev,blk_no,vnode->vfsp->blk_size);
		blk = (unsigned long *)bcache->b_data;
		blk_no = blk[offset -tot];
		brelse(bcache);

		return blk_no;
	}


	/* for triple block mapping */

	/* The block number for tripple block mapping */
	totalb = (tot)*(tot);

	blkno = inode->i_block[15];

	/*reading  triple indirect block mapping */
	bcache = bread(vnode->v_rdev,blkno,vnode->vfsp->blk_size);
	blk = (unsigned long *)bcache->b_data;
	blk_no = blk[offset - totalb];
	brelse(bcache);

	/* reading double indirect block */
	bcache = bread(vnode->v_rdev,blk_no,vnode->vfsp->blk_size);
	blk = (unsigned long *)bcache->b_data;
	blk_no  = blk[offset -totalb];
	brelse(bcache);

	/* reading single indirect block */
	bcache = bread(vnode->v_rdev,blk_no,vnode->vfsp->blk_size);
	blk = (unsigned long *)bcache->b_data;
	blk_no  = blk[offset -totalb];
	brelse(bcache);

	return blk_no;
}

#ifdef EXT2_WRITE
/* Allocating inode */
struct ext2inode *ialloc(struct vnode *vnode)
{
	struct ext2_super_block *sblock;

	sblock = &vnode->vfsp->sb.ext2sb;
}
#endif

/*
 * Get the inode attributes.
 * Vnode is Locked.
 */
void ext2_get_attr(struct vnode *vn, struct vattr *attr)
{
	struct ext2inode *ino = &vn->v_ino.ext2inode;

	attr->st_ino = vn->v_inum;
	attr->st_mode = ino->i_mode;
	attr->st_nlink = ino->i_links_count;
	attr->st_uid = ino->i_uid;
	attr->st_gid = ino->i_gid;
	attr->st_size = ino->i_size;
	attr->st_atime = ino->i_atime;
	attr->st_mtime = ino->i_mtime;
	attr->st_ctime = ino->i_ctime;

	attr->st_blksize  = vn->vfsp->blk_size;
}

/*
 * Store the inode attributes.
 * Vnode is Locked.
 */
void ext2_put_attr(struct vnode *vn, struct vattr *attr)
{
	struct ext2inode *ino = &vn->v_ino.ext2inode;

	vn->v_inum = attr->st_ino;
	ino->i_mode = attr->st_mode;
	ino->i_links_count = attr->st_nlink;
	ino->i_uid = attr->st_uid;
	ino->i_gid = attr->st_gid;
	ino->i_size = attr->st_size;
	ino->i_atime = attr->st_atime;
	ino->i_mtime = attr->st_mtime;
	ino->i_ctime = attr->st_ctime;

	//FIX: blksize and cnt
	vn->v_flag |= V_DIRTY;
}

