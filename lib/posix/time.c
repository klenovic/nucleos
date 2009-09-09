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
#include <nucleos/time.h>

time_t time(time_t *tp)
{
  message m;

  if (_syscall(PM_PROC_NR, __NR_time, &m) < 0) return( (time_t) -1);
  if (tp != (time_t *) 0) *tp = m.m2_l1;
  return(m.m2_l1);
}
