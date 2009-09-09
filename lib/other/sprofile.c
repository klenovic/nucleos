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

int sprofile(int action, int size, int freq, char *ctl_ptr, int *mem_ptr)
{
  message m;

  m.PROF_ACTION         = action;
  m.PROF_MEM_SIZE       = size;
  m.PROF_FREQ           = freq;
  m.PROF_CTL_PTR        = (void *) ctl_ptr;
  m.PROF_MEM_PTR        = (void *) mem_ptr;

  return _syscall(PM_PROC_NR, __NR_sprof, &m);
}

