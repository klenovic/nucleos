/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
gettimeofday.c
*/

#include <nucleos/time.h>
#include <lib.h>
#include <time.h>

int gettimeofday(struct timeval *__restrict tp, void *__restrict tzp)
{
  message m;

  if (_syscall(MM, GETTIMEOFDAY, &m) < 0)
  	return -1;

  tp->tv_sec = m.m2_l1;
  tp->tv_usec = m.m2_l2;

  return 0;
}

