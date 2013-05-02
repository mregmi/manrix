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
 * File: fatfs.c
 * 		FAT File System Implementation.
 */

#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#include "vfs.h"
#include "fatfs.h"

/* Index to the VFS switch array */
static int index = 0;

/*
 * Search the File using vnode as the parent. Use the Flags
 * and mode from the nameidata.
 * return the vnode, vfs in nameidata.
 */
int fat_namei(struct vnode *vn, char *path, struct nameidata *ndata)
{
	int ret;

	ret = fat_readdir(vn, path, ndata);
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
int fat_read(struct file *file, struct vnode *vn, void *buf, size_t size)
{
	struct fatinode *inode = &vn->v_ino.fatinode;
	struct bcache *bdata;
	int err = -EINVAL;
	unsigned int blkno, blkindex, toread = size, read = 0, sz = 0;
	void *bufptr = buf;
	void *dataptr;

	//sched_yield();

	/* we are already at end of File */
	if(file->f_offset >= inode->i_size)
		return 0;
/*
	do{
		blkindex = file->f_offset/vn->vfsp->blk_size;
		blkno = ext2_bmap(vn, blkindex);
		bdata = bread(vn->v_rdev, blkno, vn->vfsp->blk_size);
		if(bdata == NULL)
			return err;

		dataptr = (void *)((unsigned int)bdata->b_data + (file->f_offset % vn->vfsp->blk_size));
		if(toread < (vn->vfsp->blk_size - blkindex))
			sz = toread - read;
		else
			sz = vn->vfsp->blk_size - blkindex;

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
*/
//	printf(" %d bytes read\n", read);
//	sched_yield();

	return read;
}

int fat_write(struct file *file, struct vnode *vn, void *buf, size_t size)
{
	return 0;
}

struct vnodeops fat_vnode_ops = {
	.vop_open = nulldev,
	.vop_close = nulldev,
	.vop_read = fat_read,
	.vop_write = fat_write,
	.vop_namei = fat_namei,
	.vop_read_inode = fat_read_inode,
	.vop_get_attr = fat_get_attr,
	.vop_put_attr = fat_put_attr,
	.vop_bmap = fat_bmap
};


int fat_statvfs()
{
	return 0;
}

int fat_sync()
{
	return 0;
}

struct vfsops fat_vfs_ops = {
	.vfs_mount = fat_mount,
	.vfs_unmount = fat_unmount,
	.vfs_statvfs = fat_statvfs,
	.vfs_sync = fat_sync
};

/* Mount Operation for EXT2 File System */
int fat_mount(struct vfs *vfs, int option)
{
	struct vnode *rootvn, oper;
	int ret;


	ret = read_fat_boot(vfs);
	if(ret < 0)
		return ret;

	vfs->fat.ino_val = 10;
//	printf("Superblock read Done.");
	oper.ops = &fat_vnode_ops;
	oper.vfsp = vfs;
	rootvn = vget(&oper, 2, vfs->vfs_dev);
	rootvn->ops = &fat_vnode_ops;
	vfs->vfs_mpoint = rootvn;
	rootvn->v_type = VNODE_DIR;

	// FIX mounted time, flag etc
	return 0;
}

int fat_unmount(struct vfs *vfs)
{
	return 0;
}

/*
 * Create a VFS structure, fill the ops
 * and return
 */
struct vfs *fat_construct()
{
	struct vfs *vfs;

	vfs = (struct vfs *)malloc(sizeof(struct vfs));
	memset(vfs, 0, sizeof(struct vfs));
	vfs->ops = &fat_vfs_ops;
	vfs->fatsb.bsptr = &vfs->fatsb.fatbs;
	return vfs;
}

/* Initialize Fat File System */
int fat_init()
{
//	printf("Initializing Ext2 File System. ");
	index = register_vfs("fat", fat_construct, &fat_vfs_ops);
	return 0;
}

/* Exit EXT2 File System */
int fat_exit()
{
	return unregister_vfs(index);
}

