/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include "syslib.h"

/*===========================================================================*
 *                                sys_nice			     	     *
 *===========================================================================*/
int sys_nice(int proc, int prio)
{
  message m;

  m.m1_i1 = proc;
  m.m1_i2 = prio;
  return(_taskcall(SYSTASK, SYS_NICE, &m));
}
