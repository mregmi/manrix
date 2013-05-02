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
/*	file: math.h
 * 		mathematical declarations.
 */


#ifndef __MATH_H
#define __MATH_H

extern double acos(double);
extern double asin(double);
extern double atan(double);
extern double atan2(double, double);
extern double ceil(double);
extern double cos(double);
extern double cosh(double);
extern double exp(double);
extern double fabs(double);
extern double floor(double);
extern double fmod(double, double);
extern double frexp(double, int *);
extern double ldexp(double, int);
extern double log(double);
extern double log10(double);
extern double modf(double, double *);
extern double pow(double, double);
extern double sin(double);
extern double sinh(double);
extern double sqrt(double);
extern double tan(double);
extern double tanh(double);
extern double erf(double);
extern double erfc(double);
extern double gamma(double);
extern double hypot(double, double);
extern double j0(double);
extern double j1(double);
extern double jn(int, double);
extern double lgamma(double);
extern double y0(double);
extern double y1(double);
extern double yn(int, double);
extern int    isnan(double);
extern double acosh(double);
extern double asinh(double);
extern double atanh(double);
extern double cbrt(double);
extern double expm1(double);
extern int    ilogb(double);
extern double log1p(double);
extern double logb(double);
extern double nextafter(double, double);
extern double remainder(double, double);
extern double rint(double);
extern double scalb(double, double);

#endif /* __MATH_H  */


