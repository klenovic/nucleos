/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/lib.h>
#include <nucleos/unistd.h>

int getnprocnr(pid_t pid)
{
  kipc_msg_t m;

  m.m_data1 = pid;		/* pass pid >=0 to search for */
  m.m_data2 = 0;			/* don't pass name to search for */
  if (ktaskcall(PM_PROC_NR, KCNR_GETPROCNR, &m) < 0) return(-1);
  return(m.m_data1);		/* return search result */
}

