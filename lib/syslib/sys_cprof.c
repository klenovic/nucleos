/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/syslib.h>

/*===========================================================================*
 *                                sys_cprof				     *
 *===========================================================================*/
int sys_cprof(action, size, endpt, ctl_ptr, mem_ptr)
int action; 				/* get/reset profiling tables */
int size;				/* size of allocated memory */
endpoint_t endpt;				/* caller endpoint */
void *ctl_ptr;				/* location of info struct */
void *mem_ptr;				/* location of allocated memory */
{
  kipc_msg_t m;

  m.PROF_ACTION         = action;
  m.PROF_MEM_SIZE       = size;
  m.PROF_ENDPT		= endpt;
  m.PROF_CTL_PTR        = ctl_ptr;
  m.PROF_MEM_PTR        = mem_ptr;

  return(ktaskcall(SYSTASK, SYS_CPROF, &m));
}

