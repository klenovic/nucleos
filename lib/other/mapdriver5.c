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
  kipc_msg_t m;
  m.m_data6 = label;
  m.m_data4 = len;
  m.m_data1 = major;
  m.m_data2 = dev_style;
  m.m_data3 = force;
  if (ktaskcall(FS_PROC_NR, KCNR_MAPDRIVER, &m) < 0) return(-1);
  return(0);
}

