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
/*	file: stdlib.h
 * 		standard type definitions.
 */


#ifndef __STDLIB_H
#define __STDLIB_H

#ifndef __STDDEF_H
 #include <stddef.h>
#endif

#ifdef __LIMITS_H
 #include <limits.h>
#endif

#ifndef __MATH_H
 #include <math.h>
#endif

#ifndef __SYS_WAIT_H
 #include <sys/wait.h>
#endif

#ifndef __SIZE_T
#define __SIZE_T
typedef unsigned int size_t;
#endif

#ifndef __WCHAR_T
#define __WCHAR_T
typedef int wchar_t;
#endif /* __WCHAR_T */

#ifndef NULL
 #define NULL	((void *) 0)
#endif

#define RAND_MAX		32767
#define MB_CUR_MAX

struct div{
};

typedef struct div div_t;

struct ldiv{
};

typedef struct ldiv ldiv_t;

struct lldiv{
};

typedef struct lldiv lldiv_t;


void          _Exit(int);

long          a64l(const char *);

void          abort(void);
int           abs(int);
int           atexit(void (*)(void));
double        atof(const char *);
int           atoi(const char *);
long          atol(const char *);
long long     atoll(const char *);
void         *bsearch(const void *, const void *, size_t, size_t,
                  int (*)(const void *, const void *));
void         *calloc(size_t, size_t);
div_t         div(int, int);

double        drand48(void);
char         *ecvt(double, int, int *__restrict, int *__restrict);
double        erand48(unsigned short[3]);

void          exit(int);

char         *fcvt(double, int, int *__restrict, int *__restrict);

void          free(void *);

char         *gcvt(double, int, char *);

char         *getenv(const char *);

int           getsubopt(char **, char *const *, char **);
int           grantpt(int);
char         *initstate(unsigned, char *, size_t);
long          jrand48(unsigned short[3]);
char         *l64a(long);

long          labs(long);

void          lcong48(unsigned short[7]);

ldiv_t        ldiv(long, long);
long long     llabs(long long);
lldiv_t       lldiv(long long, long long);

long          lrand48(void);

void         *malloc(size_t);
int           mblen(const char *, size_t);
size_t        mbstowcs(wchar_t *__restrict, const char *__restrict, size_t);
int           mbtowc(wchar_t *__restrict, const char *__restrict, size_t);

char         *mktemp(char *);
int           mkstemp(char *);
long          mrand48(void);
long          nrand48(unsigned short[3]);


int           posix_memalign(void **, size_t, size_t);


int           posix_openpt(int);
char         *ptsname(int);
int           putenv(char *);

void          qsort(void *, size_t, size_t, int (*)(const void *,
                  const void *));
int           rand(void);

int           rand_r(unsigned *);


long          random(void);

void         *realloc(void *, size_t);

char         *realpath(const char *__restrict, char *__restrict);
unsigned short seed48(unsigned short[3]);


int           setenv(const char *, const char *, int);


void          setkey(const char *);
char         *setstate(const char *);

void          srand(unsigned);

void          srand48(long);
void          srandom(unsigned);

double        strtod(const char *__restrict, char **__restrict);
float         strtof(const char *__restrict, char **__restrict);
long          strtol(const char *__restrict, char **__restrict, int);
long double   strtold(const char *__restrict, char **__restrict);
long long     strtoll(const char *__restrict, char **__restrict, int);
unsigned long strtoul(const char *__restrict, char **__restrict, int);
unsigned long long
              strtoull(const char *__restrict, char **__restrict, int);
int           system(const char *);

int           unlockpt(int);


int           unsetenv(const char *);

size_t        wcstombs(char *__restrict, const wchar_t *__restrict, size_t);
int           wctomb(char *, wchar_t);




#endif  /* __STDLIB_H  */
