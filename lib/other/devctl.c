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

int devctl(int ctl_req, int proc_nr, int dev_nr, int dev_style, int force)
{
  kipc_msg_t m;
  m.m4_l1 = ctl_req;
  m.m4_l2 = proc_nr;
  m.m4_l3 = dev_nr;
  m.m4_l4 = dev_style;
  m.m4_l5 = force;
  if (ktaskcall(FS_PROC_NR, KCNR_DEVCTL, &m) < 0) return(-1);
  return(0);
}

