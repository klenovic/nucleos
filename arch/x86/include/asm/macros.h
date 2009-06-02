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
 * @file arch/x86/include/asm/macros.h
 * @brief x86 macro definitions.
 */
#ifndef __X86_ASM_MACROS_H
#define __X86_ASM_MACROS_H

#ifndef __ASSEMBLER__

/**
 * @name Define a weak symbol by using assembly (inside C code).
 */
/*@{*/
#define DEFWEAKALIAS(sweak,target) \
  __asm__(".weak " sweak           "\n" \
          ".set " sweak "," target "\n");
/*@}*/

/** 
 * @name Define a symbol by using assembly (inside C code).
 */
/*@{*/
#define DEFSYM(symbol,target) \
  __asm__(".set " symbol "," target "\n");
/*@}*/

#else /* __ASSEMBLER__ */

/** 
 * @name Define a weak symbol by using assembly (inside assembly code).
 */
/*@{*/
#define DEFWEAKALIAS(sweak,target) \
  .weak sweak; \
  .set sweak,target;
/*@}*/

/** 
 * @name Define a symbol by using assembly (assembly).
 */
/*@{*/
#define DEFSYM(symbol,target) \
  .set symbol, target;
/*@}*/

/**
 * @brief Helper macro for function definition (assembly).
 */
#define BEGIN_FUNC(name,alig,pad) \
  .p2align alig,pad;      \
  .type name, @function;  \
  .globl name;            \
name:

/**
 * @brief Helper macro for function definition (assembly).
 */
#define END_FUNC(name) .size name, .-name;

#endif /* !__ASSEMBLER__ */
#endif /* !__X86_ASM_MACROS_H */
