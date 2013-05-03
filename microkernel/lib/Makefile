#
# File:- Makefile for ManiX OS
# written by Manish Regmi (manish_regmi@hotmail.com)
#
# Makefile for easy compilation.:-)
#


OPTIONS = -nostdlib -nostdinc -O
INCLS = -I../include

CC = gcc
AS = as
ASMFILES = $(wildcard *.s)
CFILES = $(wildcard *.c)
OBJFILES = $(wildcard *.o)

all:
	@echo "Compiling Libraries.........."

	$(CC) -c $(OPTIONS) $(INCLS)  $(CFILES)

clear:
	rm $(OBJFILES)
