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

int setuid(uid_t usr)
{
  message m;

  m.m1_i1 = usr;
  return(_syscall(PM_PROC_NR, __NR_setuid, &m));
}

int seteuid(uid_t usr)
{
  message m;

  m.m1_i1 = usr;
  return(_syscall(PM_PROC_NR, __NR_seteuid, &m));
}
