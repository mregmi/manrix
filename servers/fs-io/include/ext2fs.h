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
 * File: ext2fs.h
 * 		Declaration for EXT2 File System.
 */

/*
 *  linux/include/linux/ext2_fs.h
 *
 * Copyright (C) 1992, 1993, 1994, 1995
 * Remy Card (card@masi.ibp.fr)
 * Laboratoire MASI - Institut Blaise Pascal
 * Universite Pierre et Marie Curie (Paris VI)
 *
 *  from
 *
 *  linux/include/linux/minix_fs.h
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */



#ifndef __EXT2FS_H
#define __EXT2FS_H

#ifndef __STDINT_H
	#include <stdint.h>
#endif	/* __STDINT_H */

#define EXT2_DEFAULT_PREALLOC_BLOCKS	8

/* Special Inode Numbers  */
#define EXT2_BAD_INO			1
#define EXT2_ROOT_INO			2
#define EXT2_ACL_IDX_INO		3
#define EXT2_ACL_DATA_INO		4
#define EXT2_BOOT_LOADER_INO		5
#define EXT2_UNDEL_DIR_INO		6

#define EXT2_GOOD_OLD_FIRST_INO	11

#define EXT2_SUPER_MAGIC		0xEF53	/* EXT2 Fs Magic Number */

#define EXT2_LINK_MAX			32000	/* Max count of links to the file */

/* Block Size Management */
#define EXT2_MIN_BLOCK_SIZE		1024
#define EXT2_MAX_BLOCK_SIZE		4096
#define EXT2_MIN_BLOCK_LOG_SIZE 10

#define  EXT2_BLOCK_SIZE(s)		(EXT2_MIN_BLOCK_SIZE << (s)->s_log_block_size)
#define  EXT2_ACLE_PER_BLOCK(s)		(EXT2_BLOCK_SIZE(s) / sizeof (struct ext2_acl_entry))
#define  EXT2_ADDR_PER_BLOCK(s)		(EXT2_BLOCK_SIZE(s) / sizeof (uint16_t))

#define EXT2_INODE_SIZE(s)	(((s)->s_rev_level == EXT2_GOOD_OLD_REV) ? \
				 		EXT2_GOOD_OLD_INODE_SIZE : (s)->s_inode_size)

#define EXT2_FIRST_INO(s)	(((s)->s_rev_level == EXT2_GOOD_OLD_REV) ? \
				 		EXT2_GOOD_OLD_FIRST_INO :  (s)->s_first_ino)

/* EXT2 Fragment Sizes */
#define EXT2_MIN_FRAG_SIZE		1024
#define EXT2_MAX_FRAG_SIZE		4096
#define EXT2_MIN_FRAG_LOG_SIZE	10

#define EXT2_FRAG_SIZE(s)		(EXT2_MIN_FRAG_SIZE << (s)->s_log_frag_size)
#define EXT2_FRAGS_PER_BLOCK(s)	(EXT2_BLOCK_SIZE(s) / EXT2_FRAG_SIZE(s))

/* Block Group Macros */
# define EXT2_BLOCKS_PER_GROUP(s)	((s)->s_blocks_per_group)
# define EXT2_DESC_PER_BLOCK(s)		(EXT2_BLOCK_SIZE(s) / sizeof (struct ext2_group_desc))
# define EXT2_INODES_PER_GROUP(s)	((s)->s_inodes_per_group)
# define EXT2_INODES_PER_BLOCK(s)	(EXT2_BLOCK_SIZE(s) / sizeof (struct ext2inode))
# define EXT2_BLOCK_SIZE_BITS(s)	((s)->s_log_block_size + 10)

/* Constants relative to the data blocks  */
#define	EXT2_NDIR_BLOCKS		12
#define	EXT2_IND_BLOCK			EXT2_NDIR_BLOCKS
#define	EXT2_DIND_BLOCK			(EXT2_IND_BLOCK + 1)
#define	EXT2_TIND_BLOCK			(EXT2_DIND_BLOCK + 1)
#define	EXT2_N_BLOCKS			(EXT2_TIND_BLOCK + 1)

 /*Inode flags  */
#define	EXT2_SECRM_FL			0x00000001 /* Secure deletion */
#define	EXT2_UNRM_FL				0x00000002 /* Undelete */
#define	EXT2_COMPR_FL			0x00000004 /* Compress file */
#define	EXT2_SYNC_FL				0x00000008 /* Synchronous updates */
#define	EXT2_IMMUTABLE_FL		0x00000010 /* Immutable file */
#define EXT2_APPEND_FL			0x00000020 /* writes to file may only append */
#define EXT2_NODUMP_FL			0x00000040 /* do not dump file */
#define EXT2_NOATIME_FL			0x00000080 /* do not update atime */

/* Reserved for compression usage... */
#define EXT2_DIRTY_FL				0x00000100
#define EXT2_COMPRBLK_FL			0x00000200 /* One or more compressed clusters */
#define EXT2_NOCOMP_FL				0x00000400 /* Don't compress */
#define EXT2_ECOMPR_FL				0x00000800 /* Compression error */

/* End compression flags --- maybe not all used */
#define EXT2_BTREE_FL				0x00001000 /* btree format dir */
#define EXT2_RESERVED_FL			0x80000000 /* reserved for ext2 lib */

#define EXT2_FL_USER_VISIBLE		0x00001FFF /* User visible flags */
#define EXT2_FL_USER_MODIFIABLE		0x000000FF /* User modifiable flags */

/* Codes for operating systems */
#define EXT2_OS_LINUX		0
#define EXT2_OS_HURD		1
#define EXT2_OS_MASIX		2
#define EXT2_OS_FREEBSD		3
#define EXT2_OS_LITES		4

/* Revision levels  */
#define EXT2_GOOD_OLD_REV	0	/* The good old (original) format */
#define EXT2_DYNAMIC_REV	1 	/* V2 format w/ dynamic inode sizes */

#define EXT2_CURRENT_REV	EXT2_GOOD_OLD_REV
#define EXT2_MAX_SUPP_REV	EXT2_DYNAMIC_REV

#define EXT2_GOOD_OLD_INODE_SIZE 128

/* Default values for user and/or group using reserved blocks */
#define	EXT2_DEF_RESUID		0
#define	EXT2_DEF_RESGID		0

/* Structure of a directory entry */
#define EXT2_NAME_LEN 255

/* File Format */
#define EXT2_S_IFMT			0xF000
#define EXT2_S_IFSOCK		0xC000
#define EXT2_S_IFLNK		0xA000
#define EXT2_S_IFREG		0x8000
#define EXT2_S_IFBLK		0x6000
#define EXT2_S_IFDIR		0x4000
#define EXT2_S_IFCHR		0x2000
#define EXT2_S_IFIFO		0x1000

/* access right */
#define EXT2_S_ISUID		0x0800
#define EXT2_S_ISGID		0x0400
#define EXT2_S_ISVTX		0x0200
#define EXT2_S_IRWXU		0x01C0
#define EXT2_S_IRUSR		0x0100
#define EXT2_S_IXUSR		0x0080
#define EXT2_S_IRWXG		0x0040
#define EXT2_S_IRGRP		0x0038
#define EXT2_S_IXGRP		0x0020
#define EXT2_S_IWGRP		0x0010
//#define EXT2_S_IXGRP		0x0008
#define EXT2_S_IRWXO		0x0007
#define EXT2_S_IROTH		0x0004
#define EXT2_S_IWOTH		0x0002
#define EXT2_S_IXOTH		0x0001

/* Identifies the type of file by using i_mode of inode */
/* structure as input.									*/
#define EXT2_ISDIR(mode)	((mode & EXT2_S_IFMT) == EXT2_S_IFDIR)
#define EXT2_ISLINK(mode)	((mode & EXT2_S_IFMT) == EXT2_S_IFLNK)
#define EXT2_ISBLK(mode)	((mode & EXT2_S_IFMT) == EXT2_S_IFBLK)
#define EXT2_ISSOCK(mode)	((mode & EXT2_S_IFMT) == EXT2_S_IFSOCK)
#define EXT2_ISREG(mode)	((mode & EXT2_S_IFMT) == EXT2_S_IFREG)
#define EXT2_ISCHAR(mode)	((mode & EXT2_S_IFMT) == EXT2_S_IFCHR)
#define EXT2_ISFIFO(mode)	((mode & EXT2_S_IFMT) == EXT2_S_IFIFO)

/* Identifies the type of file by using file_type of 	*/
/* directory structure as input.						*/
#define FT_ISDIR(mode)	(mode  == 0x2)
#define FT_ISLINK(mode)	(mode  == 0x7)
#define FT_ISBLK(mode)	(mode  == 0x4)
#define FT_ISSOCK(mode)	(mode  == 0x6)
#define FT_ISREG(mode)	(mode  == 0x1)
#define FT_ISCHAR(mode)	(mode  == 0x3)
#define FT_ISFIFO(mode)	(mode  == 0x5)

/* In Ext2 disk is divided into Blocks. These Blocks are divided into Groups. This   */
/* Structure shows different types of groups but it is not implemented. Not Necessary */
/*
typedef struct tagBLOCK_GROUP
{
	1. The SuperBlock
	2. The Group Descriptors
	3. The block Bitmap
	4. The Inode Bitmap
	5. The Inode Table
	6. Data Blocks and Fragments

}BLOCK_GROUP;

*/

/* The Super Block comes first in the block group */
struct ext2_super_block
{
	uint32_t	s_inodes_count;   	/* total no of inodes */
	uint32_t	s_blocks_count;   	/* total no of blocks */
	uint32_t	s_r_blocks_count; 	/* total no of blocks reserved for exclusive use  of superuser */
	uint32_t	s_free_blocks_count;	/* total no of free blocks */
	uint32_t	s_free_inodes_count;	/* total no of free inodes */
	uint32_t	s_first_data_block;	/* position of the first data block */
	uint32_t	s_log_block_size;	/* used to compute logical block size in bytes */
	uint32_t	s_log_frag_size;		/* used to compute logical fragment size  */
	uint32_t	s_blocks_per_group;	/* total number of blocks contained in the group  */
	uint32_t	s_frags_per_group;	/* total number of fragments in a group */
	uint32_t	s_inodes_per_group;	/* number of inodes in a group  */
	uint32_t	s_mtime;			/* time at which the last mount was performed */
	uint32_t	s_wtime;			/* time at which the last write was performed */
	uint16_t	s_mnt_count;		/* number of time the fs system has been mounted in r/w mode without having checked */
	uint16_t	s_max_mnt_count;	/* the max no of times the fs can be mounted in r/w mode before a check must be done */
	uint16_t	s_magic;			/* a number that identifies the fs (eg. 0xef53 for ext2) */
	uint16_t	s_state;			/* gives the state of fs (eg. 0x001 is Unmounted cleanly) */
	uint16_t	s_pad;				/* unused */
	uint16_t    s_minor_rev_level;	/*	*/
	uint32_t	s_lastcheck;		/* the time of last check performed */
	uint32_t	s_checkinterval;		/* the max possible time between checks on the fs */
	uint32_t	s_creator_os;		/* os */
	uint32_t	s_rev_level;			/* Revision level */
	uint16_t	s_def_resuid;		/* default uid for reserved blocks */
	uint16_t	s_def_regid;		/* default gid for reserved blocks */

	/* for EXT2_DYNAMIC_REV superblocks only */
	uint32_t	s_first_ino; 			/* First non-reserved inode */
	uint16_t    s_inode_size; 			/* size of inode structure */
	uint16_t	s_block_group_nr; 		/* block group # of this superblock */
	uint32_t	s_feature_compat; 		/* compatible feature set */
	uint32_t	s_feature_incompat; 	/* incompatible feature set */
	uint32_t	s_feature_ro_compat; 	/* readonly-compatible feature set */
	uint8_t		s_uuid[16];				/* 128-bit uuid for volume */
	char		s_volume_name[16]; 		/* volume name */
	char		s_last_mounted[64]; 	/* directory where last mounted */
	uint32_t	s_algorithm_usage_bitmap; /* For compression */
	uint8_t		s_prealloc_blocks;		/* Nr of blocks to try to preallocate*/
	uint8_t		s_prealloc_dir_blocks;	/* Nr to preallocate for dirs */
	uint16_t	s_padding1;
	uint32_t	s_reserved[204];		/* unused */
};

/* The Group Descriptors follow the Super Block. */
 struct ext2_group_desc
{
	uint32_t	bg_block_bitmap;	/* points to the blocks bitmap for the group */
	uint32_t	bg_inode_bitmap;	/* points to the inodes bitmap for the group */
	uint32_t	bg_inode_table;		/* points to the inode table first block     */
	uint16_t	bg_free_blocks_count;	/* number of free blocks in the group 	     */
	uint16_t	bg_free_inodes_count;	/* number of free inodes in the		     */
	uint16_t	bg_used_dirs_count;	/* number of inodes allocated to directories */
	uint16_t	bg_pad;			/* padding */
	uint32_t	bg_reserved[3];		/* reserved */
};

/* Access control structures */
struct ext2_acl_header	/* Header of Access Control Lists */
{
	uint32_t	aclh_size;
	uint32_t	aclh_file_count;
	uint32_t	aclh_acle_count;
	uint32_t	aclh_first_acle;
};

struct ext2_acl_entry	/* Access Control List Entry */
{
	uint32_t	acle_size;
	uint16_t	acle_perms;	/* Access permissions */
	uint16_t	acle_type;	/* Type of entry */
	uint16_t	acle_tag;	/* User or group identity */
	uint16_t	acle_pad1;
	uint16_t	acle_next;	/* Pointer on next entry for the  same inode or on next free entry */
};

/* Structure of an inode on the disk  */
struct ext2inode
{
	uint16_t	i_mode;		/* File mode */
	uint16_t	i_uid;		/* Low 16 bits of Owner Uid */
	uint32_t	i_size;		/* Size in bytes */
	uint32_t	i_atime;		/* Access time */
	uint32_t	i_ctime;		/* Creation time */
	uint32_t	i_mtime;		/* Modification time */
	uint32_t	i_dtime;		/* Deletion Time */
	uint16_t	i_gid;		/* Low 16 bits of Group Id */
	uint16_t	i_links_count;	/* Links count */
	uint32_t	i_blocks;		/* Blocks count */
	uint32_t	i_flags;		/* File flags */
	union {
		struct {
			uint32_t  l_i_reserved1;
		} linux1;
		struct {
			uint32_t  h_i_translator;
		} hurd1;
		struct {
			uint32_t  m_i_reserved1;
		} masix1;
	} osd1;				/* OS dependent 1 */
	uint32_t	i_block[EXT2_N_BLOCKS];/* Pointers to blocks */
	uint32_t	i_generation;	/* File version (for NFS) */
	uint32_t	i_file_acl;		/* File ACL */
	uint32_t	i_dir_acl;		/* Directory ACL */
	uint32_t	i_faddr;		/* Fragment address */
	union {
		struct {
			uint8_t	l_i_frag;	/* Fragment number */
			uint8_t	l_i_fsize;	/* Fragment size */
			uint16_t	i_pad1;
			uint16_t	l_i_uid_high;	/* these 2 fields    */
			uint16_t	l_i_gid_high;	/* were reserved2[0] */
			uint32_t	l_i_reserved2;
		} linux2;
		struct {
			uint8_t	h_i_frag;	/* Fragment number */
			uint8_t	h_i_fsize;	/* Fragment size */
			uint16_t	h_i_mode_high;
			uint16_t	h_i_uid_high;
			uint16_t	h_i_gid_high;
			uint16_t	h_i_author;
		} hurd2;
		struct {
			uint8_t	m_i_frag;	/* Fragment number */
			uint8_t	m_i_fsize;	/* Fragment size */
			uint16_t	m_pad1;
			uint32_t	m_i_reserved2[2];
		} masix2;
	} osd2;					/* OS dependent 2 */
};

/* EXT2 directory structure */
struct ext2_dir_entry {
	uint32_t	inode;			/* Inode number */
	uint16_t	rec_len;		/* Directory entry length */
	uint8_t	name_len;		/* Name length */
	uint8_t file_type;		/* File Type */
	char	name[EXT2_NAME_LEN];	/* File name */
};


struct ext2sb{
	struct ext2_super_block	ext2;
	struct ext2_group_desc  *gdes;
};

#endif
