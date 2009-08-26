/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include "syslib.h"

int sys_memset(unsigned long pattern, phys_bytes base, phys_bytes bytes)
{
/* Zero a block of data.  */
  message mess;

  if (bytes == 0L) return 0;

  mess.MEM_PTR = (char *) base;
  mess.MEM_COUNT   = bytes;
  mess.MEM_PATTERN = pattern;

  return(_taskcall(SYSTASK, SYS_MEMSET, &mess));
}

