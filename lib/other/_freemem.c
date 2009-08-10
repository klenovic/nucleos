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
#define freemem	_freemem
#include <unistd.h>


int freemem(size, base)
phys_bytes size;			/* size of mem chunk requested */
phys_bytes base;			/* base address of mem chunk */
{
  message m;
  m.m4_l1 = size;		
  m.m4_l2 = base;		
  if (_syscall(MM, FREEMEM, &m) < 0) return(-1);
  return(0);
}

