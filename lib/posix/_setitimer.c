/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* Copyright (c) 2009 Vrije Universiteit, Amsterdam.
 * See the copyright notice in file LICENSE.minix3.
 */
#include <nucleos/lib.h>
#include <nucleos/time.h>

/*
 * This is the implementation of the function to
 * invoke the interval timer setting system call.
 */
int setitimer(int which, const struct itimerval *__restrict value,
		struct itimerval *__restrict ovalue)
{
  message m;

  /* A null pointer for 'value' would make setitimer behave like getitimer,
   * which is not according to the specification, so disallow null pointers.
   */
  if (value == NULL) return(EINVAL);

  m.m1_i1 = which;
  m.m1_p1 = (char *) value;
  m.m1_p2 = (char *) ovalue;

  return _syscall(PM_PROC_NR, __NR_itimer, &m);
}
