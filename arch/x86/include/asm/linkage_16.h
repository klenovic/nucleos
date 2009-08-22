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
 * @file arch/x86/include/asm/linkage_16.h
 * @brief Data definition
 * @details Basic types upon which most other types are built.
 */
#ifndef __ASM_X86_LINKAGE_16_H
#define __ASM_X86_LINKAGE_16_H

// 16-bit code alignment
#ifdef CONFIG_X86_P2ALIGN_16
#define P2ALIGN_16 CONFIG_X86_P2ALIGN_16
#endif

#endif /* __ASM_X86_LINKAGE_16_H */
