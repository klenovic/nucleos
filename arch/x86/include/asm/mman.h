/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __ASM_X86_MMAN_H
#define __ASM_X86_MMAN_H

#include <asm-generic/mman-common.h>

/* arch specific */

/* flags argument for mmap() */
#define MAP_PREALLOC	0x0008	/* not on-demand */
#define MAP_CONTIG	0x0010	/* contiguous in physical memory */
#define MAP_LOWER16M	0x0020	/* physically below 16MB */
#define MAP_ALIGN64K	0x0040	/* physically aligned at 64kB */
#define MAP_LOWER1M	0x0080	/* physically below 1MB */

#endif /* __ASM_X86_MMAN_H */
