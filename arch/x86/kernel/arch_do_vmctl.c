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
 *   m_type:	SYS_VMCTL
 *
 * The parameters for this kernel call are:
 *   	SVMCTL_WHO	which process
 *    	SVMCTL_PARAM	set this setting (VMCTL_*)
 *    	SVMCTL_VALUE	to this value
 */

#include <kernel/system.h>
#include <nucleos/type.h>

extern u32_t kernel_cr3;

/*===========================================================================*
 *				arch_do_vmctl				     *
 *===========================================================================*/
int arch_do_vmctl(m_ptr, p)
register message *m_ptr;	/* pointer to request message */
struct proc *p;
{
  switch(m_ptr->SVMCTL_PARAM) {
	case VMCTL_I386_GETCR3:
		/* Get process CR3. */
		m_ptr->SVMCTL_VALUE = p->p_seg.p_cr3;
		return 0;
	case VMCTL_I386_SETCR3:
		/* Set process CR3. */
		if(m_ptr->SVMCTL_VALUE) {
			p->p_seg.p_cr3 = m_ptr->SVMCTL_VALUE;
			p->p_misc_flags |= MF_FULLVM;
		} else {
			p->p_seg.p_cr3 = kernel_cr3;
			p->p_misc_flags &= ~MF_FULLVM;
		}
		RTS_LOCK_UNSET(p, VMINHIBIT);
		return 0;
	case VMCTL_INCSP:
		/* Increase process SP. */
		p->p_reg.sp += m_ptr->SVMCTL_VALUE;
		return 0;
        case VMCTL_GET_PAGEFAULT:
	{
  		struct proc *rp;
		if(!(rp=pagefaults))
			return -ESRCH;
		pagefaults = rp->p_nextpagefault;
		if(!RTS_ISSET(rp, PAGEFAULT))
			minix_panic("non-PAGEFAULT process on pagefault chain",
				rp->p_endpoint);
                m_ptr->SVMCTL_PF_WHO = rp->p_endpoint;
                m_ptr->SVMCTL_PF_I386_CR2 = rp->p_pagefault.pf_virtual;
		m_ptr->SVMCTL_PF_I386_ERR = rp->p_pagefault.pf_flags;
		return 0;
	}
  }

  kprintf("arch_do_vmctl: strange param %d\n", m_ptr->SVMCTL_PARAM);
  return -EINVAL;
}
