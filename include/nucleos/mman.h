/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

#ifndef _NUCLEOS_MMAN_H
#define _NUCLEOS_MMAN_H

#include <asm/mman.h>

#if defined(__KERNEL__) || defined(__UKERNEL__)

#include <nucleos/types.h>

void *mmap(void *, size_t, int, int, int, off_t);

/* weak declaration since someone may want to overide it (see PM/VM) */
__weak int munmap(void *, size_t);
int __munmap(void *, size_t);

__weak int munmap_text(void *, size_t);
int __munmap_text(void *, size_t);

/**
 * @brief Unmap 0th page code and data
 */
void unmap_page_zero(void);

void *vm_remap(int d, int s, void *da, void *sa, size_t si);
int vm_unmap(int endpt, void *addr);
unsigned long vm_getphys(int endpt, void *addr);
u8_t vm_getrefcount(int endpt, void *addr);

#endif /* defined(__KERNEL__) || defined(__UKERNEL__) */

#endif /* _NUCLEOS_MMAN_H */
