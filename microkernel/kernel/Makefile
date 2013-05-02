#
# File:- Makefile for ManiX OS
# written by Manish Regmi (manish_regmi@hotmail.com)
#
# Makefile for easy compilation.:-)
# file = /kernel/


OPTIONS = -nostdlib -nostdinc -fno-builtin -Wall -O2
INCLS = -I. -I../ -I../include -I../include/i386

#CC = gcc
AS = as
LD = ld

	
OBJS = main.o  kernel_stdio.o id.o message.o process.o scheduling.o signal.o sync.o thread.o timer.o fd.o info.o irq.o cpu.o

OBJFILES = $(wildcard *.o)

all: kernel.o
.c.o:
	$(CC) -c $(OPTIONS) $(INCLS) $< -o $@

kernel.o: $(OBJS)
	$(LD) -r -o kernel.o $(OBJS)
clear:
	rm -f $(OBJFILES)
	rm -f *.c~ *~

