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

pid_t getnpid(endpoint_t proc_ep)
{
  kipc_msg_t m;
  m.m_data1 = proc_ep;		/* search pid for this process */
  return ktaskcall(PM_PROC_NR, KCNR_GETEPINFO, &m);
}
