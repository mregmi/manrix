#
# Init for ManRiX OS
# Copyright (C) 2004, 2005	Manish Regmi (regmi dot manish at gmail dot com)
#				            Rajesh R.C (rajee5283 at hotmail dot com)
#/mnt/manRiX/
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
#
 
LIBCDIR = ../libc/

INCL = -I. -I./include/ -I../libc/include/ -I../libc/include/arch/i386/

OBJS = init.o 
OPTIONS = -nostdlib -nostdinc -fno-builtin -Wall -O2

LD = ld
#CC = gcc
COPY = cp
STRIP = strip
OBJDUMP = objdump

ASMFILES = $(wildcard *.S)
CFILES = $(wildcard *.c)
OBJFILES = $(wildcard *.o)

all:init
.c.o:
	$(CC) -c $(OPTIONS) $(INCL) $< -o $@

init: $(OBJS)	
	$(LD) -o init -T $(LIBCDIR)gcc/i386usr.ld  $(LIBCDIR)gcc/i386.o $(OBJS) $(LIBCDIR)libc.a
	
	$(OBJDUMP) -D init > initdmp.txt
	$(COPY) init /mnt/manRiX/bin/init
	mount -o loop /mnt/e/rescue/mx/manix.img /mnt/tmp
	$(COPY) init /mnt/tmp/bin/init
	umount /mnt/tmp

clear:
	rm -f *.o *.c~ *~ init
	
