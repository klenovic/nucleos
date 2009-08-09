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
#define time	_time
#include <time.h>

time_t time(tp)
time_t *tp;
{
  message m;

  if (_syscall(MM, TIME, &m) < 0) return( (time_t) -1);
  if (tp != (time_t *) 0) *tp = m.m2_l1;
  return(m.m2_l1);
}
