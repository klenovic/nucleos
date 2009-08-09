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
#define lseek	_lseek
#include <unistd.h>

off_t lseek(fd, offset, whence)
int fd;
off_t offset;
int whence;
{
  message m;

  m.m2_i1 = fd;
  m.m2_l1 = offset;
  m.m2_i2 = whence;
  if (_syscall(FS, LSEEK, &m) < 0) return( (off_t) -1);
  return( (off_t) m.m2_l1);
}
