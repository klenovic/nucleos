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
#include <nucleos/fcntl.h>
#include <stdarg.h>
#include <nucleos/string.h>

int open(const char *name, int flags, ...)
{
  message m;

  va_list argp;
  va_start(argp, flags);

  if (flags & O_CREAT) {
	m.m1_i1 = strlen(name) + 1;
	m.m1_i2 = flags;
	m.m1_i3 = va_arg(argp, int); /* mode_t is automatically promoted to int */
	m.m1_p1 = (char *) name;
  } else {
	m.m3_i1 = strlen(name) + 1;
	m.m3_p1 = (char *) name;
	m.m3_i2 = flags;
  }
  va_end(argp);

  return (_syscall(FS_PROC_NR, __NR_open, &m));
}
