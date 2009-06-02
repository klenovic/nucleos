/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/**
 * @file arch/x86/include/asm/linkage.h
 * @brief Data definition
 * @details Basic types upon which most other types are built.
 */
#ifndef __X86_ASM_LINKAGE_H
#define __X86_ASM_LINKAGE_H

#ifdef __CODE16__
#include <asm/linkage_16.h>
#elif __CODE32__
#include <asm/linkage_32.h>
#endif

#endif /* !__X86_ASM_LINKAGE_H */
