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
#define lseek64	_lseek64
#include <unistd.h>
#include <nucleos/u64.h>

PUBLIC int lseek64(fd, offset, whence, newpos)
int fd;
u64_t offset;
int whence;
u64_t *newpos;
{
  message m;

  m.m2_i1 = fd;
  m.m2_l1 = ex64lo(offset);
  m.m2_l2 = ex64hi(offset);
  m.m2_i2 = whence;
  if (_syscall(FS, LLSEEK, &m) < 0) return -1;
  if (newpos)
	*newpos= make64(m.m2_l1, m.m2_l2);
  return 0;
}
