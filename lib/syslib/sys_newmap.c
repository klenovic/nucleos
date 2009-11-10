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

int sys_newmap(proc_ep, ptr)
endpoint_t proc_ep;		/* process whose map is to be changed */
struct mem_map *ptr;		/* pointer to new map */
{
/* A process has been assigned a new memory map.  Tell the kernel. */

  message m;

  m.PR_ENDPT = proc_ep;
  m.PR_MEM_PTR = (char *) ptr;
  return(ktaskcall(SYSTASK, SYS_NEWMAP, &m));
}
