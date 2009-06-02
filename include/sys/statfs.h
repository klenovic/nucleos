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

#ifndef _STATFS_H
#define _STATFS_H

#ifndef _TYPES_H
#include <sys/types.h>
#endif

struct statfs {
  off_t f_bsize;		/* file system block size */
};

_PROTOTYPE( int fstatfs, (int fd, struct statfs *st)		);

#endif /* _STATFS_H */
