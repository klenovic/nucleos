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
#define kill	_kill
#include <signal.h>

PUBLIC int kill(proc, sig)
int proc;			/* which process is to be sent the signal */
int sig;			/* signal number */
{
  message m;

  m.m1_i1 = proc;
  m.m1_i2 = sig;
  return(_syscall(MM, KILL, &m));
}
