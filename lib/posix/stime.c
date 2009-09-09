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
#include <nucleos/minlib.h>
#include <nucleos/time.h>

int stime(long *top)
{
  message m;

  m.m2_l1 = *top;
  return(_syscall(PM_PROC_NR, __NR_stime, &m));
}