#
# ManRiX Microkernel
# Copyright (C) 2004, 2005
# 	Manish Regmi (regmi dot manish at gmail dot com) and
# 	Rajesh R.C   (rajee5283@hotmail.com)
#
# This Program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This Program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
#
# Make File for Easy Compilation

LIBDIR = ./lib/
MMDIR  = ./mm/
I386   = ./arch/i386/
KERNEL = ./kernel/


OBJS = $(I386)i386.o $(MMDIR)vmem.o $(KERNEL)kernel.o

LD = ld
STRIP = strip
COPY = cp
REMOVE = rm

ASMFILES = $(wildcard *.S)
CFILES = $(wildcard *.c)
OBJFILES = $(wildcard *.o)

all:
	make -C arch
	make -C mm
	make -C kernel
	
	$(LD) -o kern --oformat elf32-i386 -Ttext 0x100000 -T i386.ld  $(OBJS)
	
#	$(STRIP) -s kern
	
clear:
	make -C arch clear
	make -C mm clear
	make -C kernel clear
	$(REMOVE) -r -f include/*.h~ *~ kern





