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
 * @file include/nucleos/linkage.h
 * @brief linkage defs
 */
#ifndef __NUCLEOS_LINKAGE_H
#define __NUCLEOS_LINKAGE_H

#include <asm/linkage.h>

#ifdef __CODE16__

#ifndef P2ALIGN_16
#define P2ALIGN_16  1
#endif

#elif __CODE32__

#ifndef P2ALIGN
#define P2ALIGN  2
#endif

#endif

#endif /* !__NUCLEOS_LINKAGE_H */
