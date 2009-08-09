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
#define pipe	_pipe
#include <unistd.h>

int pipe(fild)
int fild[2];
{
  message m;

  if (_syscall(FS, PIPE, &m) < 0) return(-1);
  fild[0] = m.m1_i1;
  fild[1] = m.m1_i2;
  return(0);
}
