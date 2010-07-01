/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The kernel call implemented in this file:
 *   m_type:	SYS_MEMSET
 *
 * The parameters for this kernel call are:
 *    m_data6:	MEM_PTR		(virtual address)	
 *    m_data4:	MEM_COUNT	(returns physical address)	
 *    m_data5:	MEM_PATTERN	(pattern byte to be written) 	
 */

#include <kernel/system.h>
#include <kernel/vm.h>

#if USE_MEMSET

/*===========================================================================*
 *				do_memset				     *
 *===========================================================================*/
int do_memset(m_ptr)
register kipc_msg_t *m_ptr;
{
/* Handle sys_memset(). This writes a pattern into the specified memory. */
  unsigned char c = m_ptr->MEM_PATTERN;
  vm_phys_memset((phys_bytes) m_ptr->MEM_PTR, c, (phys_bytes) m_ptr->MEM_COUNT);
  return 0;
}

#endif /* USE_MEMSET */

