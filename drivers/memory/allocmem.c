/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/lib.h>
#include <nucleos/unistd.h>

int allocmem(size, base)
phys_bytes size;			/* size of mem chunk requested */
phys_bytes *base;			/* return base address */
{
  kipc_msg_t m;
  m.m_data1 = size;		
  if (ktaskcall(PM_PROC_NR, KCNR_ALLOCMEM, &m) < 0) return(-1);
  *base = m.m_data2;
  return(0);
}

