/*
 * User Program's Linker Script for ManRiX OS
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
 */
 
OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")

OUTPUT_ARCH(i386)

ENTRY(_start)

SECTIONS
{
	. = 0x100000;	
	_text_start = . ;

	.text : { *(.text) }
	_text_end = .;

	.ctors : { *(.ctors) }
	
	. = ALIGN(4096); 
	.rodata : { *(.rodata) }

	
	. = ALIGN(4096); 
	_data_start = .;
	.data : { *(.data) }
	_data_end = .;
	
 	. = ALIGN(4096); 
	_bss_start = .;
	.bss : { *(.bss) }
	_bss_end = .;

	.dynamic  : { *(.dynamic) }	
	.comment  : { *(.comment) }
	.note  : { *(.note) }
	.eh_frame  : { *(.eh_frame) }
        .dtors  : { *(.dtors) }
	
	. = ALIGN(4096); 
        _end = .;
}
