/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/lib.h>
#include <nucleos/unistd.h>

/**
 * @brief freemem
 * @param size  size of mem chunk requested
 * @param base  base address of mem chunk
 */
int freemem(phys_bytes size, phys_bytes base)
{
  message m;
  m.m4_l1 = size;		
  m.m4_l2 = base;		
  if (ksyscall(PM_PROC_NR, __NR_freemem, &m) < 0) return(-1);
  return(0);
}

