/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* _taskcall() is the same as _syscall() except it returns negative error
 * codes directly and not in errno.  This is a better interface for MM and
 * FS.
 */

#include <lib.h>
#include <nucleos/syslib.h>

PUBLIC int _taskcall(who, syscallnr, msgptr)
int who;
int syscallnr;
register message *msgptr;
{
  int status;

  msgptr->m_type = syscallnr;
  status = _sendrec(who, msgptr);
  if (status != 0) return(status);
  return(msgptr->m_type);
}
