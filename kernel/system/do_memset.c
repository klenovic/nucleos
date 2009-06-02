/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
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
 *    m2_p1:	MEM_PTR		(virtual address)	
 *    m2_l1:	MEM_COUNT	(returns physical address)	
 *    m2_l2:	MEM_PATTERN	(size of datastructure) 	
 */

#include <kernel/system.h>

#if USE_MEMSET

/*===========================================================================*
 *				do_memset				     *
 *===========================================================================*/
PUBLIC int do_memset(m_ptr)
register message *m_ptr;
{
/* Handle sys_memset(). This writes a pattern into the specified memory. */
  unsigned long p;
  unsigned char c = m_ptr->MEM_PATTERN;
  p = c | (c << 8) | (c << 16) | (c << 24);
  phys_memset((phys_bytes) m_ptr->MEM_PTR, p, (phys_bytes) m_ptr->MEM_COUNT);
  return(OK);
}

#endif /* USE_MEMSET */

