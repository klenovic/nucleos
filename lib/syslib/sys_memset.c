/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/syslib.h>

int sys_memset(unsigned long pattern, phys_bytes base, phys_bytes bytes)
{
/* Zero a block of data.  */
  kipc_msg_t mess;

  if (bytes == 0L) return 0;

  mess.MEM_PTR = (char *) base;
  mess.MEM_COUNT   = bytes;
  mess.MEM_PATTERN = pattern;

  return(ktaskcall(SYSTASK, SYS_MEMSET, &mess));
}

