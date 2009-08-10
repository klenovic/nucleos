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
#define alarm	_alarm
#include <unistd.h>

unsigned int alarm(sec)
unsigned int sec;
{
  message m;

  m.m1_i1 = (int) sec;
  return( (unsigned) _syscall(MM, ALARM, &m));
}
