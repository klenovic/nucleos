/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/syslib.h>

/*===========================================================================*
 *                                sys_profbuf				     *
 *===========================================================================*/
int sys_profbuf(ctl_ptr, mem_ptr)
void *ctl_ptr;				/* pointer to control structure */
void *mem_ptr;				/* pointer to profiling table */
{
  message m;

  m.PROF_CTL_PTR       = ctl_ptr;
  m.PROF_MEM_PTR       = mem_ptr;

  return(_taskcall(SYSTASK, SYS_PROFBUF, &m));
}

