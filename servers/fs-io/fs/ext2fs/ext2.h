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
 * File: ext2.h
 * 		Declaration for EXT2 File System.
 */

#ifndef __FSIO_EXT2_H
#define __FSIO_EXT2_H

#include "ext2fs.h"

/* Dirent Structure */
struct ext2dirent{
	uint32_t	inum;
	uint16_t rec_len;
	uint8_t namelen;
	uint8_t file_type;
	char name[EXT2_NAME_LEN];
};

extern int ext2_namei(struct vnode *, char *, struct nameidata *);
extern int ext2_mount(struct vfs *, int);
extern int ext2_unmount(struct vfs *);

extern int ext2_read(struct file *, struct vnode *vn, void *, size_t);
extern int ext2_write(struct file *, struct vnode *vn, void *, size_t);
extern int ext2_statvfs();

extern int ext2_sync();
extern int ext2_read_inode(struct vnode *, ino_t);
extern void ext2_get_attr(struct vnode *, struct vattr *);
extern void ext2_put_attr(struct vnode *, struct vattr *);

extern int ext2fs_init();
extern int ext2fs_exit();

extern int ext2_readdir(struct vnode *, char *, struct nameidata *);
//extern uint32_t get_file_blk(struct vnode *);
//extern int check_name(char *, char *);

extern int ext2_read_sb(struct vfs *);

extern int ext2_read_inode(struct vnode *, ino_t );
extern unsigned long ext2_bmap( struct vnode *, unsigned long );
extern void ext2_get_attr(struct vnode *, struct vattr *);
extern void ext2_put_attr(struct vnode *, struct vattr *);

#endif
