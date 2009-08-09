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
#define sigpending _sigpending
#include <signal.h>

int sigpending(set)
sigset_t *set;
{
  message m;

  if (_syscall(MM, SIGPENDING, &m) < 0) return(-1);
  *set = (sigset_t) m.m2_l1;
  return(m.m_type);
}
