/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/syslib.h>
#include <stdarg.h>
#include <nucleos/unistd.h>

int sys_abort(int how, ...)
{
/* Something awful has happened.  Abandon ship. */

  message m;
  va_list ap;

  va_start(ap, how);
  if ((m.ABRT_HOW = how) == RBT_MONITOR) {
	m.ABRT_MON_ENDPT = va_arg(ap, int);
	m.ABRT_MON_ADDR = va_arg(ap, char *);
	m.ABRT_MON_LEN = va_arg(ap, size_t);
  }
  va_end(ap);

  return(ktaskcall(SYSTASK, SYS_ABORT, &m));
}
