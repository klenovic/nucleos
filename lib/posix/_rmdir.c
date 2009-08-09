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
#define rmdir	_rmdir
#include <unistd.h>

int rmdir(name)
const char *name;
{
  message m;

  _loadname(name, &m);
  return(_syscall(FS, RMDIR, &m));
}
