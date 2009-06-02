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

PUBLIC int sys_vsafecopy(struct vscp_vec *vec, int els)
{
/* Vectored variant of sys_safecopy*. */

  message copy_mess;

  copy_mess.VSCP_VEC_ADDR = (char *) vec;
  copy_mess.VSCP_VEC_SIZE = els;

  return(_taskcall(SYSTASK, SYS_VSAFECOPY, &copy_mess));

}

