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

gid_t getngid(endpoint_t proc_ep)
{
  message m;
  m.m1_i1 = proc_ep;		/* search gid for this process */
  if (_syscall(PM_PROC_NR, __NR_getepinfo, &m) < 0) return ( (gid_t) -1);
  return( (gid_t) m.m2_i2);	/* return search result */
}
