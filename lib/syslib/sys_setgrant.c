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

#include <nucleos/safecopies.h>

int sys_setgrant(cp_grant_t *grants, int ngrants)
{
  message m;

  m.SG_ADDR = (char *) grants;
  m.SG_SIZE = ngrants;

  return _taskcall(SYSTASK, SYS_SETGRANT, &m);
}
