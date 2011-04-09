/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/**
 * @file arch/x86/include/asm/types.h
 * @brief x86 specific
 */
#ifndef __ASM_X86_TYPES_H
#define __ASM_X86_TYPES_H

#include <asm-generic/types.h>

/* possible select() operation types; read, write, errors */
/* (VFS_PROC_NR/driver internal use only) */
#define SEL_RD          (1 << 0)
#define SEL_WR          (1 << 1)
#define SEL_ERR         (1 << 2)
#define SEL_NOTIFY      (1 << 3) /* not a real select operation */

#endif /* !__ASM_X86_TYPES_H */
