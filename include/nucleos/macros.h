/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __NUCLEOS_MACROS_H
#define __NUCLEOS_MACROS_H

#if !defined(__HOST__)

#include <asm/macros.h>
#define MNX(x)  x
#define INC_NUCS(x)  <x>

#else /* host's application */

// adds prefix to input
#define MNX(x)  mnx_ ## x
#define EXTERN
#define INC_NUCS(x)  <__SRCROOT__/include/x>

#define NUCS_LEVEL_TRACE  1

#define NUCS_TRACE(level,msg,args...)                             \
  if(level >= NUCS_LEVEL_TRACE) {                                 \
    fprintf(stderr,"%s:%s:%d:",__FILE__, __FUNCTION__, __LINE__); \
    fprintf(stderr, msg, ##args );                                \
  }

#endif

/** @brief Turn preprocessor symbol definition \a sym into string. */
#define MKSTR(sym)  __MKSTR2(sym)

/** @brief Helper macro for MKSTR */
#define __MKSTR2(sym) #sym

#endif /* __NUCLEOS_MACROS_H */
