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
 *                                sys_exit			     	     *
 *===========================================================================*/
int sys_exit(proc_ep)
endpoint_t proc_ep;			/* which process has exited */
{
/* A process has exited. PM tells the kernel. In addition this call can be
 * used by system processes to directly exit without passing through the
 * PM. This should be used with care to prevent inconsistent PM tables. 
 */
  message m;

  m.PR_ENDPT = proc_ep;
  return(ktaskcall(SYSTASK, SYS_EXIT, &m));
}
