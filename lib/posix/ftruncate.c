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
#include <nucleos/string.h>
#include <nucleos/unistd.h>

int ftruncate(int _fd, off_t _length)
{
  message m;
  m.m2_l1 = _length;
  m.m2_i1 = _fd;

  return(ksyscall(FS_PROC_NR, __NR_ftruncate, &m));
}
