#
# File:- Makefile for ManiX OS
# written by Manish Regmi (manish_regmi@hotmail.com)
# 
# Makefile for easy compilation.:-)
# file = /arch/i386/

OPTIONS = -nostdlib -nostdinc -fno-builtin -Wall -O2
INCLS = -I. -I../ -I../../ -I../../include  -I../../include/i386
#CC = gcc
AS = as
LD = ld

OBJS =	kernel_entry.o interrupt.o descriptors.o traps.o multiboot.o console.o \
	i386_setup.o memory.o pmap.o pic.o scheduling.o signal.o syscall.o thread.o \
	timer.o bootup.o irq.o process.o smp.o apic.o

OBJFILES = $(wildcard *.o)


all: i386.o
.c.o:
	$(CC) -c $(OPTIONS) $(INCLS)  $< -o $@

.S.o:
	$(CC) -c $(OPTIONS) $(INCLS)  $< -o $@

i386.o:	$(OBJS)
	$(LD) -r -o i386.o $(OBJS)
clear:
	rm -f $(OBJFILES)
	rm -f *.c~ *~ *.S~
