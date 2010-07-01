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

gid_t getngid(endpoint_t proc_ep)
{
  kipc_msg_t m;
  m.m_data1 = proc_ep;		/* search gid for this process */
  if (ktaskcall(PM_PROC_NR, KCNR_GETEPINFO, &m) < 0) return ( (gid_t) -1);
  return( (gid_t) m.m_data2);	/* return search result */
}
