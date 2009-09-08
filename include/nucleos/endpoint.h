/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __NUCLEOS_ENDPOINT_H
#define __NUCLEOS_ENDPOINT_H

#include <nucleos/com.h>
#include <nucleos/limits.h>

/* The point of the padding in 'generation size' is to 
 * allow for certain bogus endpoint numbers such as NONE, ANY, etc.
 *
 * The _MAX_MAGIC_PROC is defined by <nucleos/com.h>. That include
 * file defines some magic process numbers such as ANY and NONE,
 * and must never be a valid endpoint number. Therefore we make sure
 * the generation size is big enough to start the next generation
 * above the highest magic number.
 */
#define _ENDPOINT_GENERATION_SIZE (NR_TASKS+_MAX_MAGIC_PROC+1)
#define _ENDPOINT_MAX_GENERATION  (INT_MAX/_ENDPOINT_GENERATION_SIZE-1)

/* Generation + Process slot number <-> endpoint. */
#define _ENDPOINT(g, p) ((g) * _ENDPOINT_GENERATION_SIZE + (p))
#define _ENDPOINT_G(e) (((e)+NR_TASKS) / _ENDPOINT_GENERATION_SIZE)
#define _ENDPOINT_P(e) ((((e)+NR_TASKS) % _ENDPOINT_GENERATION_SIZE) - NR_TASKS)

#endif /* __NUCLEOS_ENDPOINT_H */
