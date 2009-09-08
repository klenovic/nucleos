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

#ifndef __X86_ASM_STATFS_H
#define __X86_ASM_STATFS_H

#include <nucleos/types.h>

struct statfs {
	off_t f_bsize;		/* file system block size */
};

#endif /* __X86_ASM_STATFS_H */
