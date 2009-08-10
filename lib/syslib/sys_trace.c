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

int sys_trace(req, proc_nr, addr, data_p)
int req, proc_nr;
long addr, *data_p;
{
  message m;
  int r;

  m.CTL_ENDPT = proc_nr;
  m.CTL_REQUEST = req;
  m.CTL_ADDRESS = addr;
  if (data_p) m.CTL_DATA = *data_p;
  r = _taskcall(SYSTASK, SYS_TRACE, &m);
  if (data_p) *data_p = m.CTL_DATA;
  return(r);
}
