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
#define geteuid	_geteuid
#include <unistd.h>

uid_t geteuid()
{
  message m;

  /* POSIX says that this function is always successful and that no
   * return value is reserved to indicate an error.  Minix syscalls
   * are not always successful and Minix returns the unreserved value
   * (uid_t) -1 when there is an error.
   */
  if (_syscall(MM, GETUID, &m) < 0) return ( (uid_t) -1);
  return( (uid_t) m.m2_i1);
}
