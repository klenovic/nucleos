/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/syslib.h>

/*===========================================================================*
 *                                sys_nice			     	     *
 *===========================================================================*/
int sys_nice(endpoint_t proc_ep, int prio)
{
  kipc_msg_t m;

  m.PR_ENDPT = proc_ep;
  m.PR_PRIORITY = prio;
  return(ktaskcall(SYSTASK, SYS_NICE, &m));
}
