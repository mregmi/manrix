/*
 * FS and I/O manager for ManRiX OS
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
 * File: Elf.h
 *
 * The elf file format as defined by Unix System Laboratories (USL).
 */

#ifndef __KERNEL_ELF_H
#define __KERNEL_ELF_H

#ifndef __STDINT_H
 #include <stdint.h>
#endif /* __STDINT_H */


#define EI_NIDENT	16

#define EI_MAG0		0
#define EI_MAG1		1
#define EI_MAG2		2
#define EI_MAG3		3

/* e_type values */
#define ET_NONE		0		/* No file type */
#define ET_REL		1		/* Relocatable file */
#define ET_EXEC		2		/* Executable file */
#define ET_DYN		3		/* Shared object file */
#define ET_CORE		4		/* Core file */
#define ET_LOPROC	0xff00	/* Processor-specific */
#define ET_HIPROC	0xffff	/* Processor-specific */

/* e_machine values */
#define EM_NONE		0		/* No machine */
#define EM_M32		1		/* AT&T WE 32100 */
#define EM_SPARC	2		/* SPARC */
#define EM_386		3		/* Intel 80386 */
#define EM_68K		4		/* Motorola 68000 */
#define EM_88K		5		/* Motorola 88000 */
#define EM_860		7		/* Intel 80860 */
#define EM_MIPS		8		/* MIPS RS3000 */

/* e_version value */
#define EV_NONE		0		/* Invalid version */
#define EV_CURRENT	1		/* Current version */

/* p_type values */
#define PT_NULL		0
#define PT_LOAD		1
#define PT_DYNAMIC	2
#define PT_INTERP	3
#define PT_NOTE		4
#define PT_SHLIB 	5
#define PT_PHDR 	6
#define PT_LOPROC 	0x70000000
#define PT_HIPROC 	0x7fffffff

/* p_flags values */
#define PF_X 		0x1			/* Execute */
#define PF_W		0x2			/* Write */
#define PF_R	 	0x4			/* Read */
#define PF_MASKPROC 0xf0000000 	/* Unspecified */

/* Section types: sh_type */
#define SHT_NULL 	0
#define SHT_PROGBITS 1
#define SHT_SYMTAB 	2
#define SHT_STRTAB 	3
#define SHT_RELA 	4
#define SHT_HASH 	5
#define SHT_DYNAMIC 6
#define SHT_NOTE 	7
#define SHT_NOBITS	8
#define SHT_REL 	9
#define SHT_SHLIB 	10
#define SHT_DYNSYM 	11
#define SHT_LOPROC 	0x70000000
#define SHT_HIPROC 	0x7fffffff
#define SHT_LOUSER 	0x80000000
#define SHT_HIUSER 	0xffffffff

/* Section Flags: sh_flags */
#define SHF_WRITE 	0x1
#define SHF_ALLOC 	0x2
#define SHF_EXECINSTR 0x4
#define SHF_MASKPROC 0xf0000000

typedef uint16_t	Elf32_Half;
typedef uint32_t	Elf32_Word;
typedef uint32_t	Elf32_Addr;
typedef uint32_t	Elf32_Off;
typedef int32_t 	Elf32_Sword;

typedef struct{
   unsigned char	e_ident[EI_NIDENT];
   Elf32_Half		e_type;
   Elf32_Half		e_machine;
   Elf32_Word		e_version;
   Elf32_Addr		e_entry;	/* entry point virtual address */
   Elf32_Off		e_phoff;	/* program header tables file offset in bytes. */
   Elf32_Off		e_shoff;	/* section header tables file offset in bytes. */
   Elf32_Word		e_flags;	/* processor-specific flags */
   Elf32_Half		e_ehsize;	/* ELF headers size in bytes. */
   Elf32_Half		e_phentsize;/* size in bytes of one entry in the files program header table; */
   Elf32_Half		e_phnum;	/* the number of entries in the program header table. */
   Elf32_Half		e_shentsize;/* section headers size in bytes. */
   Elf32_Half		e_shnum;	/* number of entries in the section header table. */
   Elf32_Half		e_shstrndx;	/* the section header table index of the entry
   					associated with the section name string table */

} Elf32_Ehdr;

typedef struct{
	Elf32_Word		p_type;		/* what kind of segment this array element describes */
	Elf32_Off		p_offset;	/* offset from the beginning of the file where the first
						first byte of segment lies. */
	Elf32_Addr		p_vaddr;	/* virtual address at which the first byte of the segment
						 resides */
	Elf32_Addr		p_paddr;
	Elf32_Word		p_filesz;	/* number of bytes in the file image of the segment */
	Elf32_Word		p_memsz;	/* number of bytes in the memory image of the segment; */
	Elf32_Word		p_flags;
	Elf32_Word		p_align;
} Elf32_Phdr;


struct exec_struct{
	Elf32_Ehdr header;
	Elf32_Phdr *phdr;
};

typedef struct {
	Elf32_Word		sh_name;
	Elf32_Word 		sh_type;
	Elf32_Word 		sh_flags;
	Elf32_Addr 		sh_addr;
	Elf32_Off 		sh_offset;
	Elf32_Word 		sh_size;
	Elf32_Word 		sh_link;
	Elf32_Word 		sh_info;
	Elf32_Word 		sh_addralign;
	Elf32_Word 		sh_entsize;
} Elf32_Shdr;

extern int open_elf(struct vnode *, struct vn_object *);
extern int elf_read_data(struct vnode *, struct vn_object *, void *);

#endif /* __KERNEL_ELF_H */
