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

int sys_svrctl(int proc, int request, int priv, vir_bytes argp)
{
  message m;

  m.CTL_ENDPT = proc;
  m.CTL_REQUEST = request;
  m.CTL_MM_PRIV = priv;
  m.CTL_ARG_PTR = (char *) argp;

  return _taskcall(SYSTASK, SYS_PRIVCTL, &m);
}
