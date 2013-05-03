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
 * File: balloc.c
 *			block allocation
 */

 #include <ext2fs.h>
 #include <vfs.h>
 #include <asm/bits.h>

struct bcache block_bitmap[EXT_BITMAP_CACHE] = {{NULL}} ;
unsigned int index_block_group[EXT_BITMAP_CACHE];

static void read_block_bitmap (struct vnode *vnode, unsigned long block_group,unsigned int blk_group )
{

	struct bcache *bcache;

	gde = group_desc_read(vnode->vfsp->sb.ext2sb,block_group);
	bcache = bread(vnode->v_rdev,gde->bg_block_bitmap,vnode->vfsp->blk_size);
	if(!bcache)
		return;
	index_block_group[bitmap_index] = block_group;
	block_bitmap[bitmap_index] = *bcache;
}

 static inline int load_block_bitmap(struct vnode *vnode ,unsigned int blk_group)
 {
	struct bcache tmp_bitmap ,*block_bitmap; ;
	unsigned int totblkgroup ,tmp;
	int i ,k ;
	struct ext2_super_block *sblock
	sb = vnode->vfsp->ext2sb;

	totblkgroup = sb->s_blocks_count - sb->s_first_data_block +
	EXT2_BLOCKS_PER_GROUP(sb) - 1 / EXT2_BLOCKS_PER_GROUP(sb);
	if(blk_group > totblkgroup)
		printf("out of limit");

	if(index_block_group[0] == blk_group)
			return 0;
	if(totblkgroup <= EXT_BITMAP_CACHE)
	{
	  if(block_bitmap[blk_group])
		{
			if(index_block_group[blk_group] == blk_group)
				{
						return blk_group;
				}
			else
				printf("error");
		}

	 else
		{
			read_block_bitmap(vnode,blk_group ,blk_group)
			return blk_group;

		}
	}

		/* If the known block group number is within the given array of index_block_group
		 * then the known block group is removed from the location  of array and place
		 * to the front of the array
		 *
		 */
	for(i = 0; index_block_group[i] != blk_group;i++);

	  if(index_block_group[i] == blk_group)
	   {
			  tmp  = index_block_group[i];
			  tmp_bitmap  =  block_bitmap [i];
		for( k = i ;k > 0 k--)
			{
			  index_block_group [k] = index_block_group[k-1];
			  block_bitmap[k] =  block_bitmap[k-1];
			}

			block_bitmap[0] = tmp_bitmap ;
			index_block_group[0] = tmp;
			brelse(&tmp_bitmap);
	  }
			/* If array of caches  is full then the last cache in the array is removed and
			 *	new cache is put in front of the cache array.
			 */
	  else
		{

		 brelse (inode_bitmap[EXT_BITMAP_CACHE - 1]);
		 for (j = 7; j > 0; j--)
		  {
			index_block_group[j] = index_block_group[j - 1];
			inode_bitmap[j] = inode_bitmap[j - 1];
		  }
			read_block_bitmap (vnode ,blk_group,0);
		}
			return 0;

 }






