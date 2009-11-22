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

int mapdriver5(char *label, size_t len, int major, int dev_style, int force)
{
  message m;
  m.m2_p1 = label;
  m.m2_l1 = len;
  m.m2_i1 = major;
  m.m2_i2 = dev_style;
  m.m2_i3 = force;
  if (ksyscall(FS_PROC_NR, KCNR_MAPDRIVER, &m) < 0) return(-1);
  return(0);
}

