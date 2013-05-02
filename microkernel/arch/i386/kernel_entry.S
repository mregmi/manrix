/**
 * ManRiX Microkernel
 * Copyright (C) 2004, 2005
 * 	Manish Regmi (regmi dot manish at gmail dot com) and
 * 	Rajesh R.C   (rajesh dot rc at gmail dot com)
 *
 * This Program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This Program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 **/
/**
 * File:- kernel_entry.S
 *
 * This file contains the entry point for our kernel (main file).
 * i.e our kernel starts from here.
 * Manix uses Grub as a boot loader which is probably the best boot
 * loader that exist today. since, Manix os is loaded by GRUB
 * we don't need to do things like enabling Protection, enabling
 * the A20 line etc... but paging is still disabled.
 **/

#define __ASSEMBLY__	1
#include <asm/multiboot.h>
#include <kernel/config.h>


.text

.global _start		/* Linker's entry point (see ld script)*/
.globl kernel_stack

_start:

	jmp kernel_entry
	.align  4	/* align to 32 bit boundry */

	/* Multiboot header as defined in MuliBoot Specification */
multiboot_header:
	.long  MULTIBOOT_HEADER_MAGIC		/* magic */
	.long  MULTIBOOT_HEADER_FLAGS		/* flags */
	.long  MULTIBOOT_HEADER_CHECKSUM	/* checksum */

kernel_entry:
	/* Initialize stack pointer */
	leal kernel_stack, %ecx
	addl $0xFFF, %ecx
	movl %ecx, %esp

	/* 
	 * Push the values returned by GRUB 
	 * As C calling convention, they can be accessed as
	 * function parameters
	 * EAX = magic number 
	 * EBX = Multiboot Info address 
	 */
	
	cli		/* Do not allow interrupts */
	pushl %ebx
	pushl %eax
	call multiboot_parse
	popl %eax
	popl %ebx
	
	/* Now Jump to our Main C function. */
	jmp ManRiX_main
	
	/* We will Never Return here. */
	hlt



/* Space for stack to be used by initializer. */
.data
.align 4096
kernel_stack:
	.rept 1024
	.long 0
	.endr


/**
 * The application processors start from here.
 * Actually the BSP copies the code to the lower mem
 * so that AP's can boot correctly.
 **/

#ifdef MANRIX_SMP
	.code16	/* The AP's will be in real mode */
ap_init:
	cli		/* stay safe by disabling interrupt in local processor */
	mov %cs, %ax
	mov %ax, %ds
	mov %ax, %es
	
	/* Now load the dummy gdt and ldt */
	lgdt ap_gdtr
	lidt ap_idtr
	
	/* Reload us to new descriptors */
	ljmp $0x08, $1f
 1:
 	movw $0x10, %ax
	movw %%ax, %%ds
	movw %%ax, %%es
	movw %%ax, %%ss
	
	/* Now switch to protected mode */
	movl %cr0, %eax
	incl %eax
	movl %eax, %cr0
	
	/* It should be safe to temporarily use this stack */
	movl $0xFFF, %esp
	
	/* Now we are safe to do other setup in C code */
	ljmp $0x08, ap_boot
ap_gdt:
	.quad	0x0000000000000000	/* NULL descriptor */
	.quad	0x00cf9a000000ffff	/* code */
	.quad	0x00cf92000000ffff	/* data */
	
	.word 	0
ap_gdtr:
	.word	0x20			/* limit */
	.long	ap_gdtr			/* base */
	
	.word	0
ap_idtr:
	.word	0			/* limit */
	.long 	0			/* base */

apic_init_end:

.globl ap_init
.globl ap_init_end

#endif	/* MANRIX_SMP */
