/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <lib.h>
#define setuid	_setuid
#define seteuid	_seteuid
#include <unistd.h>

PUBLIC int setuid(usr)
_mnx_Uid_t usr;
{
  message m;

  m.m1_i1 = usr;
  return(_syscall(MM, SETUID, &m));
}

PUBLIC int seteuid(usr)
_mnx_Uid_t usr;
{
  message m;

  m.m1_i1 = usr;
  return(_syscall(MM, SETEUID, &m));
}
