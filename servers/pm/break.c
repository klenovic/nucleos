/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

#include "pm.h"
#include "param.h"
#include "glo.h"
#include "mproc.h"

#include <nucleos/vm.h>

/*===========================================================================*
 *				do_brk  				     *
 *===========================================================================*/
int do_brk()
{
  int r;
/* Entry point to brk(addr) system call.  */
  r = vm_brk(mp->mp_endpoint, m_in.addr);
  mp->mp_reply.reply_ptr = (r == OK ? m_in.addr : (char *) -1);
  return r;
}

