#
# File:- Makefile for ManiX OS
# written by Manish Regmi (manish_regmi@hotmail.com)
#
# Makefile for easy compilation.:-)
# file= /mm/


OPTIONS = -nostdlib -nostdinc -fno-builtin -Wall -O2
INCLS = -I. -I../ -I../include -I../include/i386
#CC = gcc
AS = as
LD = ld

OBJS = vm_page.o vm_slab.o vm.o vm_object.o vm_map.o vm_pager.o

OBJFILES = $(wildcard *.o)

all: vmem.o
.c.o:
	$(CC) -c $(OPTIONS) $(INCLS)  $< -o $@

vmem.o: $(OBJS)	
	$(LD) -r -o vmem.o $(OBJS)

clear:
	rm -f $(OBJFILES)
	rm -f *.c~ *~

