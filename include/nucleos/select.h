/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

#ifndef _NUCLEOS_SELECT_H
#define _NUCLEOS_SELECT_H

#include <nucleos/types.h>
#include <nucleos/time.h>

#if defined(__KERNEL__) || defined(__UKERNEL__)

struct timeval;

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, struct timeval *timeout);
#endif

#endif /* _NUCLEOS_SELECT_H */

