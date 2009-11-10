/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/syslib.h>

int sys_privctl(endpoint_t proc_ep, int request, void *p)
{
  message m;

  m.CTL_ENDPT = proc_ep;
  m.CTL_REQUEST = request;
  m.CTL_ARG_PTR = p;

  return ktaskcall(SYSTASK, SYS_PRIVCTL, &m);
}

int sys_privquery_mem(endpoint_t proc_ep, phys_bytes start, phys_bytes len)
{
  message m;

  m.CTL_ENDPT = proc_ep;
  m.CTL_REQUEST = SYS_PRIV_QUERY_MEM;
  m.CTL_PHYSSTART = start;
  m.CTL_PHYSLEN = len;

  return ktaskcall(SYSTASK, SYS_PRIVCTL, &m);
}
