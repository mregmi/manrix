#
# LIBC for ManRiX OS
# Copyright (C) 2004, 2005	Manish Regmi (regmi dot manish at gmail dot com)
#				Rajesh R.C (rajee5283 at hotmail dot com)
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



OPTIONS = -nostdlib -nostdinc -fno-builtin -Wno-implicit-function-declaration -O2

LD = ld
CC = gcc

ASMFILES = $(wildcard *.S)
CFILES = $(wildcard *.c)
OBJFILES = $(wildcard *.o)

all:
	make -C signal
	make -C timer
	make -C sched
	make -C pthreads
	make -C termio
	make -C process
	make -C memory
	make -C kernel
	make -C files
	make -C ansi
	make -C gcc
	make -C posix
	
	ar -rcv libc.a ./bin/*.o
	

clear:
	make -C signal clear
	make -C timer clear
	make -C sched clear
	make -C pthreads clear
	make -C process clear
	make -C memory clear
	make -C kernel clear
	make -C files clear
	make -C ansi clear
	make -C gcc clear
	make -C termio clear
	make -C posix clear

	rm -f libc.a
	rm -f ./bin/*.o
 
