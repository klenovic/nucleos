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
#define mknod	_mknod
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

PUBLIC int mknod(name, mode, dev)
_CONST char *name;
_mnx_Mode_t mode;
Dev_t dev;
{
  message m;

  m.m1_i1 = strlen(name) + 1;
  m.m1_i2 = mode;
  m.m1_i3 = dev;
  m.m1_p1 = (char *) name;
  m.m1_p2 = (char *) ((int) 0);		/* obsolete size field */
  return(_syscall(FS, MKNOD, &m));
}
