/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* Data for fstatfs() call. */

#ifndef __NUCLEOS_STATFS_H
#define __NUCLEOS_STATFS_H

#include <nucleos/types.h>
#include <asm/statfs.h>

#if defined(__KERNEL__) || defined(__UKERNEL__)
int fstatfs(int fd, struct statfs *st);
#endif /* defined(__KERNEL__) || defined(__UKERNEL__) */

#endif /* __NUCLEOS_STATFS_H */
