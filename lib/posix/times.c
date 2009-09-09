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
#include <nucleos/times.h>
#include <nucleos/time.h>

clock_t times(struct tms *buf)
{
  message m;

  m.m4_l5 = 0;			/* return this if system is pre-1.6 */
  if (_syscall(PM_PROC_NR, __NR_times, &m) < 0) return( (clock_t) -1);
  buf->tms_utime = m.m4_l1;
  buf->tms_stime = m.m4_l2;
  buf->tms_cutime = m.m4_l3;
  buf->tms_cstime = m.m4_l4;
  return(m.m4_l5);
}
