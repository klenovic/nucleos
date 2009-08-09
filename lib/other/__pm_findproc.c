/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <lib.h>
#define _pm_findproc	__pm_findproc
#include <unistd.h>
#include <string.h>

int _pm_findproc(proc_name, proc_nr)
char *proc_name;		/* name of process to search for */
int *proc_nr;			/* return process number here */
{
  message m;

  m.m1_p1 = proc_name;
  m.m1_i1 = -1;			/* search by name */
  m.m1_i2 = strlen(proc_name) + 1;
  if (_syscall(MM, GETPROCNR, &m) < 0) return(-1);
  *proc_nr = m.m1_i1;
  return(0);
}

