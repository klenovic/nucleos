/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

#include "sysutil.h"
#include <nucleos/u64.h>
#include <nucleos/syslib.h>

/* Utility function to work directly with u64_t
 * By Antonio Mancina
 */
PUBLIC void read_tsc_64(t)
u64_t* t;
{
    u32_t lo, hi;
    read_tsc (&hi, &lo);
    *t = make64 (lo, hi);
}

