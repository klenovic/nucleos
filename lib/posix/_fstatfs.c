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
#define fstatfs	_fstatfs
#include <sys/stat.h>
#include <sys/statfs.h>

int fstatfs(int fd, struct statfs *buffer)
{
  message m;

  m.m1_i1 = fd;
  m.m1_p1 = (char *) buffer;
  return(_syscall(FS, FSTATFS, &m));
}
