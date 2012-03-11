/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __ASM_GENERIC_BITOPS_H
#define __ASM_GENERIC_BITOPS_H

/*
 * For the benefit of those who are trying to port Linux to another
 * architecture, here are some C-language equivalents.  You should
 * recode these in the native assembly language, if at all possible.
 *
 * C language equivalents written by Theodore Ts'o, 9/26/92
 */

#include <nucleos/compiler.h>

#include <asm-generic/bitops/fls.h>

#ifndef __NUCLEOS_BITOPS_H
#error only <nucleos/bitops.h> can be included directly
#endif

#endif /* __ASM_GENERIC_BITOPS_H */
