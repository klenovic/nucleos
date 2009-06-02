/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* getdma.c 
 */

#include <lib.h>
#define getdma	_getdma
#include <unistd.h>
#include <stdarg.h>

int getdma(procp, basep, sizep)
endpoint_t *procp;
phys_bytes *basep;
phys_bytes *sizep;
{
  int r;
  message m;

  r= _syscall(MM, GETDMA, &m);
  if (r == 0)
  {
	*procp= m.m2_i1;
	*basep= m.m2_l1;
	*sizep= m.m2_l2;
  }
  return r;
}
