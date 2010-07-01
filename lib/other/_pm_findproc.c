/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/lib.h>
#include <nucleos/unistd.h>
#include <nucleos/string.h>

/**
 * @brief
 * @param proc_name  name of process to search for
 * @param proc_nr  return process number here
 */
int _pm_findproc(char *proc_name, int *proc_nr)
{
  kipc_msg_t m;

  m.m_data4 = proc_name;
  m.m_data1 = -1;			/* search by name */
  m.m_data2 = strlen(proc_name) + 1;
  if (ktaskcall(PM_PROC_NR, KCNR_GETPROCNR, &m) < 0) return(-1);
  *proc_nr = m.m_data1;
  return(0);
}

