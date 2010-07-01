/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* getdma.c 
 */
#include <nucleos/lib.h>
#include <nucleos/unistd.h>
#include <stdarg.h>

int getdma(endpoint_t *procp, phys_bytes *basep, phys_bytes *sizep)
{
  int r;
  kipc_msg_t m;

  r= ktaskcall(PM_PROC_NR, KCNR_GETDMA, &m);
  if (r == 0)
  {
	*procp= m.m_data1;
	*basep= m.m_data4;
	*sizep= m.m_data5;
  }
  return r;
}
