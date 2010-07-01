/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The <times.h> header is for time times() system call. */

#ifndef __NUCLEOS_TIMES_H
#define __NUCLEOS_TIMES_H

#include <nucleos/types.h>

struct tms {
	__kernel_clock_t tms_utime;
	__kernel_clock_t tms_stime;
	__kernel_clock_t tms_cutime;
	__kernel_clock_t tms_cstime;
};

#if defined(__KERNEL__) || defined(__UKERNEL__)
/* Function Prototypes. */
clock_t times(struct tms *_buffer);
#endif /* defined(__KERNEL__) || defined(__UKERNEL__) */

#endif /* __NUCLEOS_TIMES_H */
