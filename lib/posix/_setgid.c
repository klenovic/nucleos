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
#define setgid	_setgid
#define setegid	_setegid
#include <unistd.h>

int setgid(grp)
gid_t grp;
{
  message m;

  m.m1_i1 = (int) grp;
  return(_syscall(MM, SETGID, &m));
}

int setegid(grp)
gid_t grp;
{
  message m;

  m.m1_i1 = (int) grp;
  return(_syscall(MM, SETEGID, &m));
}
