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

int getnprocnr(pid_t pid)
{
  message m;

  m.m1_i1 = pid;		/* pass pid >=0 to search for */
  m.m1_i2 = 0;			/* don't pass name to search for */
  if (ksyscall(PM_PROC_NR, KCNR_GETPROCNR, &m) < 0) return(-1);
  return(m.m1_i1);		/* return search result */
}

