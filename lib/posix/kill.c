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
#include <nucleos/signal.h>

/**
 * @brief kill
 * @param proc  which process is to be sent the signal
 * @param sig  signal number
 */
int kill(int proc, int sig)
{
  message m;

  m.m1_i1 = proc;
  m.m1_i2 = sig;
  return(_syscall(PM_PROC_NR, __NR_kill, &m));
}
