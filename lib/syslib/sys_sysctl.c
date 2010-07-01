/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

#include <nucleos/syslib.h>

int sys_sysctl(int code, char *arg1, int arg2)
{
  kipc_msg_t m;

  m.SYSCTL_CODE = code;
  m.SYSCTL_ARG1 = arg1;
  m.SYSCTL_ARG2 = arg2;

  return(ktaskcall(SYSTASK, SYS_SYSCTL, &m));

}

int sys_sysctl_stacktrace(endpoint_t ep)
{
  return sys_sysctl(SYSCTL_CODE_STACKTRACE, NULL, ep);
}

