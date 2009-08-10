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
#define fchmod	_fchmod
#include <sys/stat.h>

int fchmod(fd, mode)
int fd;
_mnx_Mode_t mode;
{
  message m;

  m.m3_i1 = fd;
  m.m3_i2 = mode;
  return(_syscall(FS, FCHMOD, &m));
}
