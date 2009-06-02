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
#include <nucleos/endpoint.h>
#define getsysinfo	_getsysinfo
#define getsysinfo_up	_getsysinfo_up
#include <nucleos/sysinfo.h>


PUBLIC int getsysinfo(who, what, where)
endpoint_t who;			/* from whom to request info */
int what;			/* what information is requested */
void *where;			/* where to put it */
{
  message m;
  m.m1_i1 = what;
  m.m1_p1 = where;
  if (_syscall(who, GETSYSINFO, &m) < 0) return(-1);
  return(0);
}

/* Unprivileged variant of getsysinfo. */
PUBLIC ssize_t getsysinfo_up(who, what, size, where)
endpoint_t who;			/* from whom to request info */
int what;			/* what information is requested */
size_t size;			/* input and output size */
void *where;			/* where to put it */
{
  message m;
  m.SIU_WHAT = what;
  m.SIU_WHERE = where;
  m.SIU_LEN = size;
  return _syscall(who, GETSYSINFO_UP, &m);
}

