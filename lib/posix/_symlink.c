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
#define symlink	_symlink
#include <string.h>
#include <unistd.h>

int symlink(name, name2)
const char *name, *name2;
{
  message m;

  m.m1_i1 = strlen(name) + 1;
  m.m1_i2 = strlen(name2) + 1;
  m.m1_p1 = (char *) name;
  m.m1_p2 = (char *) name2;
  return(_syscall(FS, SYMLINK, &m));
}
