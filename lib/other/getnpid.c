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
  message m;
  m.m1_i1 = proc_ep;		/* search pid for this process */
  return ksyscall(PM_PROC_NR, KCNR_GETEPINFO, &m);
}
