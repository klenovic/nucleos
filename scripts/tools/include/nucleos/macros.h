/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __TOOLS_NUCLEOS_MACROS_H
#define __TOOLS_NUCLEOS_MACROS_H

// adds prefix to input
#define MNX(x)  mnx_ ## x
#define NUCS_LEVEL_TRACE  1

#define NUCS_TRACE(level,msg,args...)                             \
  if(level >= NUCS_LEVEL_TRACE) {                                 \
    fprintf(stderr,"%s:%s:%d:",__FILE__, __FUNCTION__, __LINE__); \
    fprintf(stderr, msg, ##args );                                \
  }

/** @brief Helper macro for __stringify */
#define __stringify2(sym) #sym

/** @brief Turn preprocessor symbol definition \a sym into string. */
#define stringify(sym)  __stringify2(sym)

#endif /* __NUCLEOS_MACROS_H */
