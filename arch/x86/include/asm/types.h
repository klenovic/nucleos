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
 * @dir arch/x86/include/asm Architecturally dependent declarations and definitions
 * @file arch/x86/include/asm/types.h
 * @brief Data definition
 * @details Basic types upon which most other types are built.
 */
#ifndef __X86_ASM_TYPES_H
#define __X86_ASM_TYPES_H

#define __UINT64_T
/** @brief Unsigned 64-bit integer type */
  typedef unsigned int __attribute__((__mode__(__DI__))) __uint64_t;

#define __UINT32_T  
/** @brief Unsigned 32-bit integer type */
  typedef unsigned int __uint32_t;

#define __UINT16_T
/** @brief Unsigned 16-bit integer type */
  typedef unsigned short __uint16_t;

#define __UINT8_T
/** @brief Unsigned 8-bit integer type */
  typedef unsigned char __uint8_t;

#define __INT64_T
/** @brief Signed 64-bit integer type */
  typedef signed int __attribute__((__mode__(__DI__))) __int64_t;

#define __INT32_T
/** @brief Signed 32-bit integer type */
  typedef signed int __int32_t;

#define __INT16_T
/** @brief Signed 16-bit integer type */
  typedef signed short __int16_t;

#define __INT8_T
/** @brief Signed 8-bit integer type */
  typedef signed char	__int8_t;

#endif /* !__X86_ASM_TYPES_H */
