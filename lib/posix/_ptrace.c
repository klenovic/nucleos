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
#define ptrace	_ptrace
#include <unistd.h>

long ptrace(req, pid, addr, data)
int req;
pid_t pid;
long addr;
long data;
{
  message m;

  m.m2_i1 = pid;
  m.m2_i2 = req;
  m.m2_l1 = addr;
  m.m2_l2 = data;
  if (_syscall(MM, PTRACE, &m) < 0) return(-1);

  /* There was no error, but -1 is a legal return value.  Clear errno if
   * necessary to distinguish this case.  _syscall has set errno to nonzero
   * for the error case.
   */
  if (m.m2_l2 == -1) errno = 0;
  return(m.m2_l2);
}
