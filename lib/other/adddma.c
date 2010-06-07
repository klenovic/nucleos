/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* adddma.c 
 */
#include <nucleos/lib.h>
#include <nucleos/unistd.h>
#include <stdarg.h>

int adddma(endpoint_t proc_e, phys_bytes start, phys_bytes size)
{
  message m;

  m.m2_i1= proc_e;
  m.m2_l1= start;
  m.m2_l2= size;

  return ktaskcall(PM_PROC_NR, KCNR_ADDDMA, &m);
}
