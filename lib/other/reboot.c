/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* reboot.c - Systemcall interface to mm/signal.c::do_reboot()

   author: Edvard Tuinder  v892231@si.hhs.NL
 */

#include <nucleos/lib.h>
#include <nucleos/unistd.h>
#include <stdarg.h>

int reboot(int how, ...)
{
  message m;
  va_list ap;

  va_start(ap, how);
  if ((m.m1_i1 = how) == RBT_MONITOR) {
	m.m1_p1 = va_arg(ap, char *);
	m.m1_i2 = va_arg(ap, size_t);
  }
  va_end(ap);

  return ksyscall(PM_PROC_NR, __NR_reboot, &m);
}
