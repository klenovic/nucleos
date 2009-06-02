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
 * @file arch/x86/include/asm/code16gcc.h
 * @brief Writing 16-bit code.
 * @note Include this file before gcc emit any code.
 * See \a as manual for differences between .\a code16gcc and .\a code16
 * Use `__NOAUTO_OP16_PREFIX__' if you do want to add address and size
 * prefixes automatically by gcc (see gcc manual affected instructions).
 */
#ifndef __X86_ASM_CODE16GCC_H
#define __X86_ASM_CODE16GCC_H

#ifndef __ASSEMBLER__
  __asm__(".code16gcc");
#else	/* from assembler source */

  #ifndef __NOAUTO_OP16_PREFIX__
    .code16gcc
  #else  /* __NOAUTO_OP16_PREFIX__ */
    .code16
  #endif /* __NOAUTO_OP16_PREFIX__ */

#endif /* __ASSEMBLER__ */

#endif /* !__X86_ASM_CODE16GCC_H */
