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
#define stime	_stime
#include <nucleos/minlib.h>
#include <time.h>

int stime(top)
long *top;
{
  message m;

  m.m2_l1 = *top;
  return(_syscall(MM, STIME, &m));
}
