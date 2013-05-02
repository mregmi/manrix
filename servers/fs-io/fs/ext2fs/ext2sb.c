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
 * File: ext2sb.c
 * 		EXT2 File System SuperBlock, Group Desc related operations.
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "vfs.h"
#include "ext2fs.h"
#include "bcache.h"
#include "blkdev.h"

/* Read the group descriptors */
static inline int read_group_desc(struct vfs *vfs, int index, struct ext2_group_desc *desc)
{
	uint32_t ngroups, gsize;
	unsigned int nreads, xfer;
	struct ext2_super_block *sb = &vfs->sb.ext2sb;
	struct bcache *cache;
	
	ngroups = sb->s_blocks_count/EXT2_BLOCKS_PER_GROUP(sb);
	gsize = ngroups * sizeof(struct ext2_group_desc);
	nreads = (gsize + (EXT2_BLOCK_SIZE(sb) - 1))/EXT2_BLOCK_SIZE(sb);
	do{
		cache = bread(vfs->vfs_dev, index, vfs->blk_size);
		if(!cache)
			return -EINTR;
		if(gsize > EXT2_BLOCK_SIZE(sb))
			xfer = EXT2_BLOCK_SIZE(sb);
		else
			xfer = gsize;
		memcpy((char *)desc, cache->b_data, xfer);
//		printf("transferred %u %d ", gsize, EXT2_BLOCK_SIZE(sb));
		gsize -= EXT2_BLOCK_SIZE(sb);
		desc = (struct ext2_group_desc *)((unsigned int)desc + xfer);
		index++;
		brelse(cache);
	}while(--nreads);
}

/*
 * We dont know about the FS,
 * So doing a direct read
 */
int ext2_read_sb(struct vfs *vfs)
{
	struct ext2_super_block sb;
	struct ext2_group_desc *gd;
	int ret;

	int index = 1, ngroups;
	
//	printf("Reading Superblock. ");
	
	//FIXME: Read the superblock
	ret = blk_read(vfs->vfs_dev, 1, sizeof(struct ext2_super_block), (void *)&sb);
	if(ret < 0)
		return ret;
		
	if(sb.s_magic != EXT2_SUPER_MAGIC)
		return -EBADMAGIC;
	
	vfs->blk_size = EXT2_BLOCK_SIZE(&sb);

	ngroups = sb.s_blocks_count/EXT2_BLOCKS_PER_GROUP(&sb);

	gd = (struct ext2_group_desc *) malloc(sizeof(struct ext2_group_desc) * ngroups);
	if(!gd)
		return -ENOMEM;
	
	memset(gd, 0, sizeof(struct ext2_group_desc) * ngroups);
	if(vfs->blk_size <= 1024)
		index = 2;
	
	memcpy((char *)&vfs->sb.ext2sb, (char *)&sb, sizeof(struct ext2_super_block));	
	
	read_group_desc(vfs, index, gd);	
	vfs->sb.ext2gdesc = gd;

	
//	printf("\nDone magic %x bg itable = %d ino count %d block %d",sb.s_magic, gd->bg_inode_table, gd->bg_free_inodes_count, EXT2_BLOCK_SIZE(&sb));

	return 0;
}
