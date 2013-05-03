/*
 * Libc for ManRiX OS
 * Copyright (C) 2004, 2005
 * 	Manish Regmi (regmi dot manish at gmail dot com) and
 * 	Rajesh R.C   (rajesh dot rc at gmail dot com)
 *
 * This Program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This Program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
/*
 *    This file is part of the GNU C Library.
 */


#ifndef __STRING_H
#define __STRING_H

#include "config.h"

#ifdef USE_ARCH_STRING
 #include <asm/string.h>
#endif

#ifndef __SYS_TYPES_H
#include <sys/types.h>
#endif

#ifndef strchrnul
#define strchrnul __strchrnul
#endif

extern char * __strchrnul (const char *s, int c_in);

/*
 * memcpy - Copy one area of memory to another
 */
// extern void *memcpy(void *__restrict, const void *__restrict, size_t);

/*
 * memchr - Find a character in an area of memory.
 */

// extern void *memchr(const void *, int, size_t);

 /*
  * memcmp - Compare two areas of memory
  */
 //extern int  memcmp(const void *, const void *, size_t);

/*
 * memmove - Copy one area of memory to another
 */
 //extern void *memmove(void *, const void *, size_t);

/*
 * memset - Fill a region of memory with the given value
 */
 //extern void *memset(void *, int, size_t);

 /* strcpy - copy a string
  *
  */
 //extern char *strcpy(char *__restrict, const char *__restrict);

 /*
  * strncpy - copy part of a string
  */
 //extern char *strncpy(char *__restrict, const char *__restrict, size_t);

 /*
  * strcat - concatenate two strings
  */
 //extern char *strcat(char *__restrict, const char *__restrict);

 /*
  * strncat - concatenate a string with part of another
  */
 //extern char *strncat(char *__restrict, const char *__restrict, size_t);

 /*
  *	strcmp - compare two strings
  */
 //extern int  strcmp(const char *, const char *);

 /*
  * strncmp - compare part of two strings
  */
 //extern int  strncmp(const char *, const char *, size_t);

 /*
  * strchr - string scanning operation
  */
 //extern  char  *strchr(const char *, int);

 /*
  * strrchr - string scanning operation
  */
 //extern char  *strrchr(const char *, int);

 /*
  * strlen - get string length
  */

 //extern size_t strlen(const char *);

 /*
  * strspn - get length of a substring
  */

 //extern size_t strspn(const char *, const char *);

 /*
  * strpbrk - scan a string for a byte
  */
 //extern char   *strpbrk(const char *, const char *);

 /*
  * strtok- split string into tokens
  */
 //extern char   *strtok(char *__restrict, const char *__restrict);

 /*
  * strstr - find a substring
  */
 //extern char   *strstr(const char *, const char *);


 #endif  /*__STRING_H  */
