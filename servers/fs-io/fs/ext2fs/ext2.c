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
 * 		EXT2 File System Implementation.
 */

#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#include "vfs.h"
#include "bcache.h"

#include "ext2.h"

/* Index to the VFS switch array */
static int index = 0;

/*
 * Search the File using vnode as the parent. Use the Flags
 * and mode from the nameidata.
 * return the vnode, vfs in nameidata.
 */
int ext2_namei(struct vnode *vn, char *path, struct nameidata *ndata)
{
	int ret;

	ret = ext2_readdir(vn, path, ndata);
	if(ret < 0)
		goto nofile;
	if(ret > 0)
		goto end;

	if((ndata->access & O_CREAT) || (ndata->access & O_EXCL))
		return -EEXIST;

	return ret;

nofile:
	if(ndata->vn == NULL)
		goto end;
	if((ndata->vn->v_type == VNODE_DIR)
		&& (ndata->access & O_CREAT))
		;//FIXME: Create a file and send its inode
end:
	return ret;
}



/* Read the data from the file */
int ext2_read(struct file *file, struct vnode *vn, void *buf, size_t size)
{
	struct ext2inode *inode = &vn->v_ino.ext2inode;
	struct bcache *bdata;
	int err = -EINVAL;
	unsigned int blkno, blkindex, toread = size, read = 0, sz = 0, off;
	void *bufptr = buf;
	void *dataptr;
	
	//sched_yield();
	
	/* we are already at end of File */
	if(file->f_offset >= inode->i_size)
		return 0;
	
	do{
		blkindex = file->f_offset/vn->vfsp->blk_size;
		blkno = ext2_bmap(vn, blkindex);
		bdata = bread(vn->v_rdev, blkno, vn->vfsp->blk_size);
		if(bdata == NULL)
			return err;
			
		off = file->f_offset % vn->vfsp->blk_size;
		dataptr = (void *)((unsigned int)bdata->b_data + off);
		if(toread < (vn->vfsp->blk_size - off))
			sz = toread;
		else
			sz = vn->vfsp->blk_size - off;
		
		if((file->f_offset + sz) > inode->i_size)
			sz = inode->i_size - file->f_offset;
		
		memcpy(bufptr, dataptr, sz);
		brelse(bdata);
		file->f_offset += sz;
		read += sz;
		if(file->f_offset >= inode->i_size)
			break;
		bufptr = (void *)((unsigned long)bufptr + sz);
		toread -= sz;
	} while(read < size);

//	printf(" %d bytes read %s\n", read, buf);
//	sched_yield();
		
	return read;
}

int ext2_write(struct file *file, struct vnode *vn, void *buf, size_t size)
{
	return 0;
}

struct vnodeops ext2_vnode_ops = {
	.vop_open = nulldev,
	.vop_close = nulldev,
	.vop_read = ext2_read,
	.vop_write = ext2_write,
	.vop_namei = ext2_namei,
	.vop_read_inode = ext2_read_inode,
	.vop_get_attr = ext2_get_attr,
	.vop_put_attr = ext2_put_attr,
	.vop_bmap = ext2_bmap
};


int ext2_statvfs()
{
	return 0;
}

int ext2_sync()
{
	return 0;
}

struct vfsops ext2_vfs_ops = {
	.vfs_mount = ext2_mount,
	.vfs_unmount = ext2_unmount,
	.vfs_statvfs = ext2_statvfs,
	.vfs_sync = ext2_sync
};

/* Mount Operation for EXT2 File System */
int ext2_mount(struct vfs *vfs, int option)
{
//	struct ext2_super_block *sb;
	struct vnode *rootvn, oper;
	int ret;

//	vfs->blk_size = get_blkdev_size(vfs->blk_size);
	ret = ext2_read_sb(vfs);
//	printf("ret %d \n", ret);
	if(ret < 0)
		return ret;

//	printf("Superblock read Done.");
	oper.ops = &ext2_vnode_ops;
	oper.vfsp = vfs;
	rootvn = vget(&oper, EXT2_ROOT_INO, vfs->vfs_dev);
	rootvn->ops = &ext2_vnode_ops;
	vfs->vfs_mpoint = rootvn;
	rootvn->v_type = VNODE_DIR;

	// FIX mounted time, flag etc
	return 0;
}

int ext2_unmount(struct vfs *vfs)
{
	free(vfs->sb.ext2gdesc);

	return 0;
}
/*
 * Create a VFS structure, fill the ops
 * and return
 */
struct vfs *ext2fs_construct()
{
	struct vfs *vfs;

	vfs = (struct vfs *)malloc(sizeof(struct vfs));
	memset(vfs, 0, sizeof(struct vfs));
	vfs->ops = &ext2_vfs_ops;

	return vfs;
}

/* Initialize EXT2 File System */
int ext2fs_init()
{
//	printf("Initializing Ext2 File System. ");
	index = register_vfs("ext2", ext2fs_construct, &ext2_vfs_ops);
	return 0;
}

/* Exit EXT2 File System */
int ext2fs_exit()
{
	return unregister_vfs(index);
}

