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

int sys_kill(proc_ep, signr)
endpoint_t proc_ep;		/* which proc_ep has exited */
int signr;			/* signal number: 1 - 16 */
{
/* A proc_ep has to be signaled via PM_PROC_NR.  Tell the kernel. */
  kipc_msg_t m;

  m.SIG_ENDPT = proc_ep;
  m.SIG_NUMBER = signr;
  return(ktaskcall(SYSTASK, SYS_KILL, &m));
}

