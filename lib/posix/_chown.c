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
#define chown	_chown
#include <string.h>
#include <unistd.h>

PUBLIC int chown(name, owner, grp)
_CONST char *name;
_mnx_Uid_t owner;
_mnx_Gid_t grp;
{
  message m;

  m.m1_i1 = strlen(name) + 1;
  m.m1_i2 = owner;
  m.m1_i3 = grp;
  m.m1_p1 = (char *) name;
  return(_syscall(FS, CHOWN, &m));
}
