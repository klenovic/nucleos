/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The kernel call implemented in this file:
 *   m_type:	SYS_MAPDMA
 *
 * The parameters for this kernel call are:
 *    m_data5:	CP_SRC_ADDR	(virtual address)	
 *    m_data7:	CP_NR_BYTES	(size of datastructure) 	
 */

#include <kernel/system.h>

/*==========================================================================*
 *				do_mapdma				    *
 *==========================================================================*/
int do_mapdma(m_ptr)
register kipc_msg_t *m_ptr;	/* pointer to request message */
{
	int r;
	endpoint_t proc_e;
	int proc_p;
	vir_bytes base, size;
	phys_bytes phys_base;
	struct proc *proc;
	kipc_msg_t m;

	proc_e = m_ptr->CP_SRC_ENDPT;
	base= m_ptr->CP_SRC_ADDR;
	size= m_ptr->CP_NR_BYTES;

	if (!isokendpt(proc_e, &proc_p))
		return(-EINVAL);

	proc = proc_addr(proc_p);

        phys_base= umap_virtual(proc, D, base, size);
        if (!phys_base)
        {
                printk("do_mapdma: umap_virtual failed\n");
		return -EFAULT;
	}

	m_ptr->CP_DST_ADDR = phys_base;
	return 0;
}
