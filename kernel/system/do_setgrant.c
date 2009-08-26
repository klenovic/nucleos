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
 *   m_type:	SYS_SETGRANT
 *
 * The parameters for this kernel call are:
 *      SG_ADDR	address of grant table in own address space
 *	SG_SIZE	number of entries
 */

#include <kernel/system.h>
#include <nucleos/safecopies.h>

/*===========================================================================*
 *				do_setgrant				     *
 *===========================================================================*/
int do_setgrant(m_ptr)
message *m_ptr;
{
	struct proc *rp;
	int r;

	/* Who wants to set a parameter? */
	rp = proc_addr(who_p);

	/* Copy grant table set in priv. struct. */
	if (RTS_ISSET(rp, NO_PRIV) || !(priv(rp))) {
		r = -EPERM;
	} else {
		_K_SET_GRANT_TABLE(rp, 
			(vir_bytes) m_ptr->SG_ADDR,
			m_ptr->SG_SIZE);
		r = 0;
	}

	return r;
}
