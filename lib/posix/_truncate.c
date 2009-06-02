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
#include <string.h>
#define truncate	_truncate
#define ftruncate	_ftruncate
#include <unistd.h>

PUBLIC int truncate(const char *_path, off_t _length)
{
  message m;
  m.m2_p1 = (char *) _path;
  m.m2_i1 = strlen(_path)+1;
  m.m2_l1 = _length;

  return(_syscall(FS, TRUNCATE, &m));
}

PUBLIC int ftruncate(int _fd, off_t _length)
{
  message m;
  m.m2_l1 = _length;
  m.m2_i1 = _fd;

  return(_syscall(FS, FTRUNCATE, &m));
}
