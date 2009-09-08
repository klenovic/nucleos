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
#include <nucleos/string.h>
#include <stdlib.h>
#include <nucleos/unistd.h>

int mknod(const char *name, mode_t mode, dev_t dev)
{
  message m;

  m.m1_i1 = strlen(name) + 1;
  m.m1_i2 = mode;
  m.m1_i3 = dev;
  m.m1_p1 = (char *) name;
  m.m1_p2 = (char *) ((int) 0);		/* obsolete size field */
  return(_syscall(FS_PROC_NR, __NR_mknod, &m));
}
