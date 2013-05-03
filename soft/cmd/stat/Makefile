#
# sh for ManRiX OS
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
 
LIBCDIR = ../../libc/

INCL = -I. -I./include/ -I../../libc/include/ -I../../libc/include/arch/i386/

OBJS = stat.o 
OPTIONS = -nostdlib -nostdinc -fno-builtin -Wno-implicit-function-declaration -O2

LD = ld
CC = gcc
COPY = cp
STRIP = strip
OBJDUMP = objdump

ASMFILES = $(wildcard *.S)
CFILES = $(wildcard *.c)
OBJFILES = $(wildcard *.o)

all:stat
.c.o:
	$(CC) -c $(OPTIONS) $(INCL) $< -o $@

stat: $(OBJS)	
	$(LD) -o stat -T $(LIBCDIR)gcc/i386usr.ld $(LIBCDIR)/gcc/i386.o $(OBJS) $(LIBCDIR)libc.a
	cp stat ../bin/	

clear:
	rm -f *.o *.c~ *~ stat
	
