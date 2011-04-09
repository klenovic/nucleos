/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __ASM_X86_MINLIB_H
#define __ASM_X86_MINLIB_H

void _cpuid(u32_t eax_in, u32_t *eax, u32_t *ebx, u32_t *ecx, u32_t *edx);

#endif /* __ASM_X86_MINLIB_H */
