/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/**
 * @file include/nucleos/nucleos.h
 * @brief contains some often-used function prototypes, macros, ...
 */
#ifndef __NUCLEOS_NUCLEOS_H
#define __NUCLEOS_NUCLEOS_H

#define CHAR_BIT	8	/* # bits in a char */
#define CHAR_MIN	-128	/* minimum value of a char */
#define CHAR_MAX	127	/* maximum value of a char */
#define SCHAR_MIN	-128	/* minimum value of a signed char */
#define SCHAR_MAX	127	/* maximum value of a signed char */
#define UCHAR_MAX	255	/* maximum value of an unsigned char */
#define USHORT_MAX	((u16)(~0U))
#define SHORT_MAX	((s16)(USHORT_MAX>>1))
#define SHORT_MIN	(-SHORT_MAX - 1)
#define INT_MAX		((int)(~0U>>1))
#define INT_MIN		(-INT_MAX - 1)
#define UINT_MAX	(~0U)
#define LONG_MAX	((long)(~0UL>>1))
#define LONG_MIN	(-LONG_MAX - 1)
#define ULONG_MAX	(~0UL)
#define LLONG_MAX	((long long)(~0ULL>>1))
#define LLONG_MIN	(-LLONG_MAX - 1)
#define ULLONG_MAX	(~0ULL)

#endif /* __NUCLEOS_NUCLEOS_H */
