/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The <math.h> header contains prototypes for mathematical functions. */

#ifndef _MATH_H
#define _MATH_H

#ifndef _ANSI_H
#include <ansi.h>
#endif

#define HUGE_VAL	(__huge_val())		/* may be infinity */

/* Function Prototypes. */
double __huge_val(void);
int __IsNan(double _x);

double acos(double _x);
double asin(double _x);
double atan(double _x);
double atan2(double _y, double _x);
double ceil(double _x);
double cos(double _x);
double cosh(double _x);
double exp(double _x);
double fabs(double _x);
double floor(double _x);
double fmod(double _x, double _y);
double frexp(double _x, int *_exp);
double ldexp(double _x, int _exp);
double log(double _x);
double log10(double _x);
double modf(double _x, double *_iptr);
double pow(double _x, double _y);
double rint(double _x);
double sin(double _x);
double sinh(double _x);
double sqrt(double _x);
double tan(double _x);
double tanh(double _x);
double hypot(double _x, double _y);

#ifdef _POSIX_SOURCE	/* STD-C? */
#include <mathconst.h>
#endif

#endif /* _MATH_H */
