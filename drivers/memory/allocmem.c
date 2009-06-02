/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <lib.h>
#include <unistd.h>

PUBLIC int allocmem(size, base)
phys_bytes size;			/* size of mem chunk requested */
phys_bytes *base;			/* return base address */
{
  message m;
  m.m4_l1 = size;		
  if (_syscall(MM, ALLOCMEM, &m) < 0) return(-1);
  *base = m.m4_l2;
  return(0);
}

