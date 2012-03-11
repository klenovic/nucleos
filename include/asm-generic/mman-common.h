/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

#ifndef __ASM_GENERIC_MMAN_COMMON_H
#define __ASM_GENERIC_MMAN_COMMON_H

/* prot argument for mmap() */
#define PROT_NONE	0x00	/* no permissions */
#define PROT_READ	0x01	/* pages can be read */
#define PROT_WRITE	0x02	/* pages can be written */
#define PROT_EXEC	0x04	/* pages can be executed */

/* flags argument for mmap() */
#define MAP_SHARED	0x0001	/* share changes */
#define MAP_PRIVATE	0x0002	/* changes are private */
#define MAP_ANONYMOUS	0x0004	/* anonymous memory */

/* mmap() error return */
#define MAP_FAILED	((void *)-1)

#endif /* __NUCLEOS_MMAN_COMMON_H */
