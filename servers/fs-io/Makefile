#
# FSIO Manager for ManRiX OS
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
 
IODIR = ./io/
FSDIR  = ./fs/
GCCDIR = ../../gcc/
LIBCDIR = ../../soft/libc/

INCL = -I. -I./include/ -I../../soft/libc/include/ -I../../soft/libc/include/arch/i386/
OBJS = main.o msg.o kernel_helper.o $(IODIR)io.o $(FSDIR)fs.o 
OPTIONS = -nostdlib -nostdinc -fno-builtin -Wall -O2

LD = ld
#CC = gcc
COPY = cp
STRIP = strip
OBJDUMP = objdump
SEP_IO = yes

all:
	make -C fs
	make -C io
	make ml

ml:fsiomngr
.c.o:
	$(CC) -c $(OPTIONS) $(INCL) $< -o $@

fsiomngr: $(OBJS)	
	$(LD) -o fsiomngr -T $(LIBCDIR)gcc/i386usr.ld  $(OBJS) $(LIBCDIR)/gcc/i386.o $(LIBCDIR)libc.a
	
clear:
	make -C fs clear
	make -C io clear
	rm -f *.o *.c~ *~ fsiomngr
	
