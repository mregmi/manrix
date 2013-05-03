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
 * 	File: ialloc.c
 * 		EXT2 File System Inode related operations.
 */

#ifdef EXT2_WRITE

#include <vfs.h>
#include <list.h>
#include <ext2fs.h>
#include <bcache.h>
#include <asm/bits.h>
#include <stdio.h>


struct bcache inode_bitmap[EXT_BITMAP_CACHE] = {{NULL}} ;

unsigned int block_group_index[EXT_BITMAP_CACHE];

static void read_inode_bitmap (struct vnode *vnode, unsigned int block_group,unsigned int bitmap_index )
{

	struct bcache *bcache;

	gde = group_desc_read(vnode->vfsp->sb.ext2sb,block_group);
	bcache = bread(vnode->v_rdev,gde->bg_inode_bitmap,vnode->vfsp->blk_size);
	if(!bcache)
		return;
	block_group_index[bitmap_index] = block_group;
	inode_bitmap[bitmap_index] = *bcache;
}

/*
 *This function loads the inode bitmap of a specified block group and
 * returns the cache position in which bitmap is found
 */

static inline int load_inode_bitmap(struct vnode *vnode,unsigned int blk_group)
{
	struct bcache tmp_bitmap ,*inode_bitmap; ;
	unsigned int totblkgroup ,tmp;
	int i ,k ;
	struct ext2_super_block *sb;
	sb = vnode->vfsp->sb.ext2sb;

	totblkgroup = sb->s_blocks_count - sb->s_first_data_block + /
	EXT2_BLOCKS_PER_GROUP(sb) - 1 / EXT2_BLOCKS_PER_GROUP(sb);


	if(blk_group > totblkgroup)
		printf("out of limit");

	if(block_group_index[0] == blk_group)
		return 0;

	if(totblkgroup <= EXT_BITMAP_CACHE)
	{
		if(inode_bitmap[blk_group])
		 {

			if(block_group_index[blk_group] == blk_group)
			{
					return blk_group;
			}
			else
				printf("error");
		 }
		else
		{
			read_inode_bitmap(vnode,blk_group ,blk_group)
			return blk_group;
		}
	}

	/* If the known block group number is within the given array of block_group_index
	 * then the known block group is removed from the location  of array and place
	 * to the front of the array
	 *
	 */
	for(i = 0; block_group_index[i] != blk_group;i++);

		if(block_group_index[i] == blk_group)
		  {

			tmp  = block_group_index[i];
			tmp_bitmap  =  inode_bitmap[i];

			for( k = i ;k > 0 k--)
				{
					block_group_index[k] = block_group_index[k-1];
					inode_bitmap[k] =  inode_bitmap[k-1];
				}

				inode_bitmap[0] = tmp_bitmap ;
				block_group_index[0] = tmp;
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
				block_group_index[j] = block_group_index[j - 1];
				inode_bitmap[j] = inode_bitmap[j - 1];
			}
				read_inode_bitmap (vnode ,blk_group,0);
	  	 }
		return 0;
}

/*Allocate inode */
struct ext2inode *alloc_inode(struct fd *fd, struct vnode *vnode)
{
	struct ext2_super_block *sblock
	sblock = vnode->vfsp->sb.ext2sb;

	struct ext2_group_desc ,*gde = NULL ,gde1 = NULL;
	struct bcache *bcache , bcache1;
	struct ext2_inode *inode = &vnode->v_ino.ext2inode;
	int i,j,index;
	unsigned int groups_count,group = 0;,avefreei;



	groups_count = sblock->s_blocks_count - sblock->s_first_data_block +
	EXT2_BLOCKS_PER_GROUP(sblock) - 1 /EXT2_BLOCKS_PER_GROUP(sblock);

//	group = (vnode->v_info-1) / EXT2_INODES_PER_GROUP(sblock);
 /*
	 * If the new inode is a directory allocate the dirextory in a block
	 * group which have the maximum number of free blocks or have the greater average
	 * number of free inodes in a block groups
	 */
	if(EXT2_ISDIR(vnode->v_type)
	   {
		 avefreei = sblock->s_free_inodes_count / groups_count ;

		 for(group = 0; group < groups_count; group)
		 {

		  gde1 = group_desc_read(vnode,group);
		  gde = group_desc_read(vnode ,(group + 1));
		  if(gde1->bg_free_inode_counts
		     &&(gde1->bg_free_inode_counts > avefreei))
		     {
				 if(gde1->bg_free_blocks_counts >
				    gde->bg_free_blocks_count)
				    {
						 gde = gde1;
						 break;
				    }
		  	 }

		 }


		gde->bg_used_dirs_count ++;

	}

	else
	{
		/* Allocate new inode in parent directory*/
		i = group;
		gde1 = group_desc_read(vnode ,i);

		if(gde1->bg_free_inode_count)
		    gde = gde1;

		else
		{
			for(j = 1; j < groups_count  j << 1)
			{
				i +=j;
				if( i > = groups_count)
					i -= groups_count;
				gde1 = group_desc_read(vnode,i);
				if(gde1->bg_free_inode_count){
			  		gde = gde1;
					break;
				}
			}
		}


		/* linear search */
		if(!gde)
		{
		  for (j = 2; j < groups_count; j++)
		  {
			  i = i + 1;
		      if (i >= groups_count)
				      i = 0;
		  	  gde1 = group_desc_read(vnode,i);
		    	if (gde1->bg_free_inodes_count)
		    	{
				   gde = gde1;
				   break;
		   		}
			}

		}
	}



	index = load_inode_bitmap(vnode,i);
	bcache = &inode_bitmap[index];
	i = ffz_bit((unsigned long*)bcache->b_data,EXT2_INODES_PER_GROUP(sblock));
	set_bit((unsigned long*)bcache->data,i);

	bcache->mode = B_DIRTY;
	gde->bg_free_inode_count--;
	sblock->s_free_inodes_count--;
	vnode->flag = V_DIRTY;

	inode = read_inode_index(vnode, gde, i, inode);

	/*Initialize inode */
	inode->i_mode = vnode->v_type;
	inode->uid = fd->uid;
	inode->gid = fd->gid;
	inode->i_block = 0;
	inode->i_mtime = inode->i_atime = inode->i_ctime = ;
	inode->i_dtime =0;
	inode->i_flags = vnode->flag;
	inode->i_faddr = 0;
	inode->i_frag = 0;
	inode->i_fsize = 0;
	inode->i_file_acl = 0;
	inode->i_dir_acl = 0;
	inode->i_dtime = 0;

	return inode;
}

/*Free inode  */

void free_inode(struct vnode *vnode)
{
	struct ext2group_desc *gds ;
	struct ext2inode *inode ;
	struct bcache *bcache;
	int blk_group ,index ;

	block_group = (vnode->v_inum - 1) / EXT2_INODES_PER_GROUP(vnode->vfsp->ext2sb);
	gds = group_desc_read(vnode->vfsp->ext2sb,blk_group);
	read_inode(vnode ,vnode->v_inum);
	inode = vnode->v_info.ext2_info;
	if(!inode)
	  return;
	if (!inode->i_dev) {
	  printf ("ext2_free_inode: inode has no device\n");
	  return;
	  }

	if(EXT2_ISDIR(vnode->v_type)
		gds->bg_used_dirs_count --;
	gds->bg_free_inode_count++;
	vnode->vfsp->ext2sb->s_free_inodes_count++;
	index = load_inode_bitmap(vnode,blk_group);
	bcache = &inode_bitmap[index];
	clear_bit((unsigned long*)bcache->data,index);
	brelse(bcache);

}

#endif
