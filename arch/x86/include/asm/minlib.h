/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __X86_ASM_MINLIB_H
#define __X86_ASM_MINLIB_H

#ifndef _ANSI_H
#include <ansi.h>
#endif

_PROTOTYPE(int _cpuid, (u32_t eax_in, u32_t *eax, u32_t *ebx, u32_t *ecx, u32_t *edx));

#endif
