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

int sys_trace(req, proc_ep, addr, data_p)
int req;
endpoint_t proc_ep;
long addr, *data_p;
{
  kipc_msg_t m;
  int r;

  m.CTL_ENDPT = proc_ep;
  m.CTL_REQUEST = req;
  m.CTL_ADDRESS = addr;
  if (data_p) m.CTL_DATA = *data_p;
  r = ktaskcall(SYSTASK, SYS_TRACE, &m);
  if (data_p) *data_p = m.CTL_DATA;
  return(r);
}
