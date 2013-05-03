#
# FSIO Manager for ManRiX OS
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
#

EXT2DIR = ./ext2fs/
FATDIR = ./fat/
BINDIR = ./binaries/
PARTDIR = ./partitions/

INCL = -I. -I../include/ -I../../../soft/libc/include/ -I../../../soft/libc/include/arch/i386/

OPTIONS = -nostdlib -nostdinc -fno-builtin -Wall -O2 

#CC = gcc
AS = as
LD = ld

OBJS = bcache.o file.o vfs.o vfs_namei.o $(EXT2DIR)ext2fs.o $(BINDIR)bin.o

ASMFILES = $(wildcard *.S)
CFILES = $(wildcard *.c)
OBJFILES = $(wildcard *.o)

all:
	make -C binaries
	make -C ext2fs
#	make -C fat
	make mfs

mfs: fs.o
.c.o:
	$(CC) -c $(OPTIONS) $(INCL)  $< -o $@

fs.o: $(OBJS)	
	$(LD) -r -o fs.o $(OBJS)

clear:
	make -C binaries clear
	make -C ext2fs clear
	make -C fat clear
	rm -f $(OBJFILES)
	rm -f *.c~ *.o *~


