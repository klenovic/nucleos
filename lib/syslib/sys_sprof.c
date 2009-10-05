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

#ifdef CONFIG_DEBUG_KERNEL_STATS_PROFILE

/*===========================================================================*
 *                                sys_sprof				     *
 *===========================================================================*/
int sys_sprof(action, size, freq, endpt, ctl_ptr, mem_ptr)
int action; 				/* start/stop profiling */
int size;				/* available profiling memory */
int freq;				/* sample frequency */
endpoint_t endpt;			/* caller endpoint */
void *ctl_ptr;				/* location of info struct */
void *mem_ptr;				/* location of profiling memory */
{
  message m;

  m.PROF_ACTION         = action;
  m.PROF_MEM_SIZE       = size;
  m.PROF_FREQ           = freq;
  m.PROF_ENDPT		= endpt;
  m.PROF_CTL_PTR        = ctl_ptr;
  m.PROF_MEM_PTR        = mem_ptr;

  return(_taskcall(SYSTASK, SYS_SPROF, &m));
}

#endif

