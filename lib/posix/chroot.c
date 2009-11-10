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
#include <nucleos/unistd.h>

int chroot(const char *name)
{
  message m;

  m.m3_i1 = strlen(name) + 1;
  m.m3_p1 = (char *) name;

  return(ksyscall(FS_PROC_NR, __NR_chroot, &m));
}
