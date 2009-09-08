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
#include <nucleos/string.h>

/**
 * @brief
 * @param proc_name  name of process to search for
 * @param proc_nr  return process number here
 */
int _pm_findproc(char *proc_name, int *proc_nr)
{
  message m;

  m.m1_p1 = proc_name;
  m.m1_i1 = -1;			/* search by name */
  m.m1_i2 = strlen(proc_name) + 1;
  if (_syscall(PM_PROC_NR, __NR_getprocnr, &m) < 0) return(-1);
  *proc_nr = m.m1_i1;
  return(0);
}

