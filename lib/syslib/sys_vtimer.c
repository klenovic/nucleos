/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* Copyright (c) 2009, Vrije Universiteit, Amsterdam.
 * See the copyright notice in file LICENSE.minix3.
 */
#include <nucleos/syslib.h>

int sys_vtimer(proc, which, newval, oldval)
endpoint_t proc;		/* proc to retrieve/set the timer for */
int which;			/* timer to retrieve/set */
clock_t *newval;		/* if non-NULL, set to this new value */
clock_t *oldval;		/* if non-NULL, old value is stored here */
{
  message m;
  int r;

  m.VT_ENDPT = proc;
  m.VT_WHICH = which;
  if (newval != NULL) {
      m.VT_SET = 1;
      m.VT_VALUE = *newval;
  } else {
      m.VT_SET = 0;
  }

  r = ktaskcall(SYSTASK, SYS_VTIMER, &m);

  if (oldval != NULL) {
      *oldval = m.VT_VALUE;
  }

  return(r);
}
