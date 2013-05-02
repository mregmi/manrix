#
# LIBC for XXX OS
# Copyright (C) 2004, 2005	Manish Regmi (regmi dot manish at gmail dot com)
#				            Rajesh R.C (rajee5283 at hotmail dot com)
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 

INCLS = -I. -I../include/ -I../include/arch/i386/


OBJS =  mman.o malloc/malloc.o
	
OPTIONS = -nostdlib -nostdinc -fno-builtin -Wall -O2 -DHAVE_CONFIG_H

LD = ld
#CC = gcc

ASMFILES = $(wildcard *.S)
CFILES = $(wildcard *.c)
OBJFILES = $(wildcard *.o)

all: 
	make -C malloc
	make ml
ml:memoryob.o
.c.o:
	$(CC) -c $(OPTIONS) $(INCLS) $< -o $@
	
memoryob.o: $(OBJS)
	#$(LD) -r -o memoryob.o $(OBJS)
	cp -f $(OBJS) ../bin/

clear:
	make -C malloc clear
	rm -f *.o *.c~
