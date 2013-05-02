#
# LIBC for XXX OS
 

INCLS = -I. -I../../include/ -I../../include/arch/i386/

OPT_FLAGS  = -O #-g
WARN_FLAGS = # -Wall
SH_FLAGS   = -shared

# Flags for the test programs
T_FLAGS   = -DUSE_MALLOC=1 -DMALLOC_HOOKS -DTEST=1

# Flags for the compilation of ptmalloc.c
M_FLAGS   = -DTHREAD_STATS=1 -DMALLOC_HOOKS #-DMALLOC_DEBUG=1

# Flags for the threads package configuration.  Adjust this for your
# architecture, or see the platform-specific targets below.
# Valid discerning symbols are:
# USE_PTHREADS=1 for Posix threads
# USE_THR=1      for Solaris threads
# USE_SPROC=1    for SGI sprocs
THR_FLAGS = -DUSE_PTHREADS=1 -DUSE_TSD_DATA_HACK -D_REENTRANT -DHAVE_CONFIG_H
THR_LIBS  = -lpthread

RM        = rm -f

# Don't need it for the Linux C library 6, see `glibc' target below.
MALLOC = ptmalloc.o

T_SUF =
TESTS = t-test1$(T_SUF) t-test2$(T_SUF)

OPTIONS = -nostdlib -nostdinc -fno-builtin -Wall -O2

CFLAGS = $(OPT_FLAGS) $(WARN_FLAGS) $(THR_FLAGS) $(OPTIONS)

OBJS = ptmalloc.o
	


LD = ld
#CC = gcc

ASMFILES = $(wildcard *.S)
CFILES = $(wildcard *.c)
OBJFILES = $(wildcard *.o)

all: malloc.o
.c.o:
	$(CC) -c $(CFLAGS) $(INCLS) $< -o $@

malloc.o: $(OBJS)
	$(LD) -r -o malloc.o $(OBJS)

clear:
	$(RM) *.o *.c~
