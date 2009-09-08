/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* linux */
#ifndef __ASM_GENERIC_SWAB_H
#define __ASM_GENERIC_SWAB_H

#include <asm/bitsperlong.h>

/*
 * 32 bit architectures typically (but not always) want to
 * set __SWAB_64_THRU_32__. In user space, this is only
 * valid if the compiler supports 64 bit data types.
 */
#if __BITS_PER_LONG == 32
#if defined(__GNUC__) && !defined(__STRICT_ANSI__) || defined(__KERNEL__)
#define __SWAB_64_THRU_32__
#endif
#endif

#endif /* __ASM_GENERIC_SWAB_H */
