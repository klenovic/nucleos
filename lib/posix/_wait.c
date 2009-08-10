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
#define wait	_wait
#include <sys/wait.h>

pid_t wait(status)
int *status;
{
  message m;

  if (_syscall(MM, WAIT, &m) < 0) return(-1);
  if (status != 0) *status = m.m2_i1;
  return(m.m_type);
}
