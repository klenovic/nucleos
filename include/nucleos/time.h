/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __NUCLEOS_TIME_H
#define __NUCLEOS_TIME_H

#include <nucleos/types.h>

/* Open Group Base Specifications Issue 6 (not complete) */
struct timeval
{
	long /*time_t*/ tv_sec;
	long /*useconds_t*/ tv_usec;
};

struct timezone {
	int     tz_minuteswest; /* minutes west of Greenwich */
	int     tz_dsttime;     /* type of dst correction */
};

int gettimeofday(struct timeval *_RESTRICT tp, void *_RESTRICT tzp);

/* Compatibility with other Unix systems */
int settimeofday(const struct timeval *tp, const void *tzp);

#define NFDBITS			__NFDBITS

#define FD_SETSIZE		__FD_SETSIZE
#define FD_ZERO(fdsetp)		__FD_ZERO(fdsetp)
#define FD_SET(fd,fdsetp)	__FD_SET(fd,fdsetp)
#define FD_CLR(fd,fdsetp)	__FD_CLR(fd,fdsetp)
#define FD_ISSET(fd,fdsetp)	__FD_ISSET(fd,fdsetp)

#endif /* __NUCLEOS_TIME_H */
