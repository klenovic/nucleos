/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* deldma.c 
 */
#include <nucleos/lib.h>
#include <nucleos/unistd.h>
#include <stdarg.h>

int deldma(endpoint_t proc_e, phys_bytes start, phys_bytes size)
{
  kipc_msg_t m;

  m.m_data1= proc_e;
  m.m_data4= start;
  m.m_data5= size;

  return ktaskcall(PM_PROC_NR, KCNR_DELDMA, &m);
}
