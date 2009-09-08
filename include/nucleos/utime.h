/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The <nucleos/utime.h> header is used for the utime() system call. */

#ifndef __NUCLEOS_UTIME_H
#define __NUCLEOS_UTIME_H

#include <nucleos/types.h>

struct utimbuf {
	__kernel_time_t actime;		/* access time */
	__kernel_time_t modtime;	/* modification time */
};

#if defined(__KERNEL__) || defined(__UKERNEL__)
/* Function Prototypes. */
int utime(const char *_path, const struct utimbuf *_times);
#endif /* defined(__KERNEL__) || defined(__UKERNEL__) */

#endif /* __NUCLEOS_UTIME_H */
