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

uid_t getnuid(endpoint_t proc_ep)
{
  message m;
  m.m1_i1 = proc_ep;		/* search uid for this process */
  if (_syscall(PM_PROC_NR, __NR_getepinfo, &m) < 0) return ( (uid_t) -1);
  return( (uid_t) m.m2_i1);	/* return search result */
}
