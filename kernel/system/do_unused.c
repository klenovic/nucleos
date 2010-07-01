/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* This file provides a catch-all handler for unused kernel calls. A kernel 
 * call may be unused when it is not defined or when it is disabled in the
 * kernel's configuration.
 */
#include <kernel/system.h>

/*===========================================================================*
 *			          do_unused				     *
 *===========================================================================*/
int do_unused(m)
kipc_msg_t *m;				/* pointer to request message */
{
  kprintf("SYSTEM: got unused request %d from %d\n", m->m_type, m->m_source);
  return(-EBADREQUEST);			/* illegal message type */
}
