# compiler and Linker Flags

CC = gcc
LD = ld
STRIP = strip
COPY = cp
REMOVE = rm

LDFLAGS = -melf_i386
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -Wall -O2 
