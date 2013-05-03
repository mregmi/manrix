/*
 * FS and I/O manager for XXX OS
 * Copyright (C) 2004, 2005	Manish Regmi (regmi dot manish at gmail dot com)
 *				            Rajesh R.C (rajee5283 at hotmail dot com)
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
 * File: elf.c
 *		The elf file format.
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <mrx/pager.h>

#include "vfs.h"
#include "elf.h"
#include "bcache.h"

#define checkmagic(e)	\
	((e->e_ident[EI_MAG0] == 0x7F) && \
	(e->e_ident[EI_MAG1] == 'E') &&	\
	(e->e_ident[EI_MAG2] == 'L') &&	\
	(e->e_ident[EI_MAG3] == 'F'))

/* Read the file header */
static inline struct exec_struct *read_header(struct vnode *vn, struct vn_object *obj)
{
	struct bcache *bcache, *bcache1;
	struct exec_struct *exec;
	Elf32_Ehdr  *hdr;
	Elf32_Shdr *shdr, *nshdr;
	Elf32_Phdr *phdr;
	int phsize, i, size, off;
	unsigned long block, addr;

	if(!vn || !vn->ops || !vn->ops->vop_bmap)
		return NULL;

	block = vn->ops->vop_bmap(vn, 0);

	bcache = bread(vn->v_rdev, block, vn->vfsp->blk_size);
	if(!bcache)
		return NULL;

	hdr = (Elf32_Ehdr  *)bcache->b_data;
	if(!checkmagic(hdr))
	{
		printf("Invalid magic number\n");
		return NULL;
	}

	phsize = hdr->e_phentsize * hdr->e_phnum;
//	printf(" headersize is %d phoff = %d %d ", hdr->e_ehsize, hdr->e_phoff, hdr->e_phnum);
	off = hdr->e_shoff/vn->vfsp->blk_size;
	if(off != 0)
	{
		block = vn->ops->vop_bmap(vn, off);
		bcache1 = bread(vn->v_rdev, block, vn->vfsp->blk_size);
		if(!bcache1)
		{
			exec = NULL;
			goto end1;
		}
		nshdr = (Elf32_Shdr *) ((uint32_t)bcache1->b_data + (hdr->e_shoff % vn->vfsp->blk_size));
	}
	else
		nshdr = (Elf32_Shdr *) ((uint32_t)hdr + hdr->e_shoff);
//	printf("Now going to noload loop shoff %X shdr sz %x type %d\n", hdr->e_shoff, shdr->sh_size, shdr->sh_type);
	for(i = 0; i < hdr->e_shnum; i++)
	{
		shdr = (Elf32_Shdr *) ((uint32_t)nshdr + (hdr->e_shentsize * i));
		addr = shdr->sh_addr;
//		printf("addr  %x type %x ", shdr->sh_addr, shdr->sh_type);
		if((shdr->sh_type == SHT_NOBITS) && (shdr->sh_flags & (SHF_ALLOC | SHF_WRITE)))
		{
			obj->noload_start = addr;
			obj->noload_end = addr + shdr->sh_size;
//			printf("\t\t\tNOLOAD found at %x size %d. \n", obj->noload_start, shdr->sh_size);
			break;
		}
	}

	exec = (struct exec_struct *)malloc(sizeof(Elf32_Ehdr) + sizeof(void *));
	exec->header = *hdr;
	exec->phdr = (Elf32_Phdr *) malloc(phsize);
//	exec->phdr = (Elf32_Phdr *)((unsigned long)exec + hdr->e_phoff);
	phdr = (Elf32_Phdr *)((unsigned long)hdr + hdr->e_phoff);
	memcpy(exec->phdr , phdr, phsize);
//	printf("EXEC is %x phd %x", exec, exec->phdr->p_vaddr);

	if(off != 0)
		brelse(bcache1);

end1:
	brelse(bcache);
	return exec;
}

/* Read the file */
static inline int read_file(struct vnode *vnode, struct vn_object *vn, void *data, Elf32_Phdr  *phdr)
{
	ssize_t size;
	unsigned long block, off, offset = vn->va - phdr->p_vaddr;;
	struct bcache *buffer;
	char *ptr = (char *)data;

/*	if(vn->blk_size >= vn->page_size)
		size = vn->blk_size;
	else
		size = vn->page_size;
//	printf("address is %X phdr is  %X ", phdr, phdr->p_offset);
	size = size/vnode->vfsp->blk_size;*/
	size = vn->size/vnode->vfsp->blk_size;
	off = offset/vnode->vfsp->blk_size;
	if(phdr->p_offset >= vnode->vfsp->blk_size)
		off = off+(phdr->p_offset/vnode->vfsp->blk_size);

	do{
		block = vnode->ops->vop_bmap(vnode, off);
//		printf("Block is %d off is %d ", block, off);
		buffer = bread(vnode->v_rdev, block, vnode->vfsp->blk_size);
		if(!buffer)
			return -1;

		memcpy(ptr, buffer->b_data, vnode->vfsp->blk_size);
		ptr = (void *)((unsigned long)ptr + vnode->vfsp->blk_size);
		brelse(buffer);
		off++;

	}while(--size);

	return 0;
}

/* Open the elf file */
int open_elf(struct vnode *vnode, struct vn_object *obj)
{
	struct exec_struct *exec;

	exec = read_header(vnode, obj);
	if(!exec)
		return -1;

	vnode->data = exec;
	obj->va = exec->header.e_entry;

	return 0;
}

/* Read the data from the file */
int elf_read_data(struct vnode *vnode, struct vn_object *obj, void *data)
{
	Elf32_Ehdr  *hdr;
	Elf32_Phdr  *phdr;
	size_t	size;
	int index;
	struct exec_struct *exec;

//	printf("vnode is %d ", vnode->v_inum);
	index = 0;
	exec = (struct exec_struct *)vnode->data;
	hdr = &exec->header;
//	printf("data is %X hdrsz %d ", vnode->data, hdr->e_ehsize);
	//addr = (Elf32_Addr)hdr + hdr->e_phoff;
	do{
		phdr = (Elf32_Phdr  *)((unsigned long)exec->phdr + (hdr->e_phentsize * index));
//		printf("Read va %x phdvr %x  sz %x ", obj->va, phdr->p_vaddr, phdr->p_filesz);

		if((obj->va >= phdr->p_vaddr) && (obj->va <= (phdr->p_vaddr + phdr->p_filesz)))
			goto done;
		index++;
	}while(index < hdr->e_phnum);
	printf("Not Found %X ", obj->va);
	if(obj->blk_size >= obj->page_size)
		size = obj->blk_size;
	else
		size = obj->page_size;
//	memset(data, 0, size);
	return 0;
done:
//	printf("Reading start %x end %x  off = %d ", phdr->p_paddr, phdr->p_paddr + phdr->p_filesz,phdr->p_offset);

//	addr = obj->va - phdr->p_vaddr;

	return read_file(vnode, obj, data, phdr);
}
