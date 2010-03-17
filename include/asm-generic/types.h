/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __ASM_GENERIC_TYPES_H
#define __ASM_GENERIC_TYPES_H
/*
 * int-ll64 is used practically everywhere now,
 * so use it as a reasonable default.
 */
#include <asm-generic/int-ll64.h>

#ifndef __ASSEMBLY__

typedef int endpoint_t;	/* process identifier */

#endif /* __ASSEMBLY__ */

#endif /* __ASM_GENERIC_TYPES_H */
