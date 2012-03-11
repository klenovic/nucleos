/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __ASM_X86_ERRNO_H
#define __ASM_X86_ERRNO_H

#include <asm-generic/errno.h>

#if defined(__KERNEL__) || defined(__UKERNEL__)
/* Don't show outside the kernel/ukernel */
extern int errno;
#endif

#endif /* __ASM_X86_ERRNO_H */
