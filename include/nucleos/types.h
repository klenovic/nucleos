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
 * @dir include/nucleos Architecturally independent declarations and definitions
 * @file include/nucleos/types.h
 * @brief Data types (architecturally independent)
 */
#ifndef __NUCLEOS_TYPES_H
#define __NUCLEOS_TYPES_H

#include <asm/types.h>

#ifdef __UINT64_T
/**@brief Unsigned 64-bit integer type */
  typedef __uint64_t uint64_t;
#endif

#ifdef __UINT32_T
/** @brief Unsigned 32-bit integer type */
  typedef __uint32_t uint32_t;
#endif

#ifdef __UINT16_T
/** @brief Unsigned 16-bit integer type */
  typedef __uint16_t uint16_t;
#endif

#ifdef __UINT8_T
/** @brief Unsigned 8-bit integer type */
  typedef __uint8_t	uint8_t;
#endif

#ifdef __INT64_T
/** @brief Signed 64-bit integer type */
  typedef __int64_t int64_t;
#endif

#ifdef __INT32_T
/** @brief Signed 32-bit integer type */
  typedef __int32_t int32_t;
#endif

#ifdef __INT16_T
/** @brief Signed 16-bit integer type */
  typedef __int16_t int16_t;
#endif

#ifdef __INT8_T
/** @brief Signed 8-bit integer type */
  typedef __int8_t int8_t;
#endif

#ifndef __BOOL_T
#define __BOOL_T
/** @brief boolean type */
  typedef _Bool bool;
#endif

#endif /* __NUCLEOS_TYPES_H */
