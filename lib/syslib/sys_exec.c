/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include "syslib.h"

PUBLIC int sys_exec(proc, ptr, prog_name, initpc)
int proc;			/* process that did exec */
char *ptr;			/* new stack pointer */
char *prog_name;		/* name of the new program */
vir_bytes initpc;
{
/* A process has exec'd.  Tell the kernel. */

  message m;

  m.PR_ENDPT = proc;
  m.PR_STACK_PTR = ptr;
  m.PR_NAME_PTR = prog_name;
  m.PR_IP_PTR = (char *)initpc;
  return(_taskcall(SYSTASK, SYS_EXEC, &m));
}
