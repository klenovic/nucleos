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
#define getnpid	_getnpid
#include <unistd.h>

pid_t getnpid(int proc_nr)
{
  message m;
  m.m1_i1 = proc_nr;		/* search pid for this process */
  if (_syscall(MM, GETPID, &m) < 0) return ( (pid_t) -1);
  return( (pid_t) m.m2_i2);	/* return search result */
}
