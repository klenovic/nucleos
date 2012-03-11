/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The kernel call implemented in this file:
 *   m_type:	SYS_STIME
 *
 * The parameters for this kernel call are:
 *    m_data3:	T_BOOTTIME
 */

#include <kernel/system.h>

#include <nucleos/endpoint.h>

/*===========================================================================*
 *				do_stime				     *
 *===========================================================================*/
int do_stime(m_ptr)
register kipc_msg_t *m_ptr;	/* pointer to request message */
{
  boottime= m_ptr->T_BOOTTIME;
  return 0;
}
