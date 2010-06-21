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
 *   m_type:	SYS_UMAP
 *
 * The parameters for this kernel call are:
 *    m_data3:	CP_SRC_PROC_NR	(process number)	
 *    m_data1:	CP_SRC_SPACE	(segment where address is: T, D, or S)
 *    m_data5:	CP_SRC_ADDR	(virtual address)	
 *    m_data6:	CP_DST_ADDR	(returns physical address)	
 *    m_data7:	CP_NR_BYTES	(size of datastructure) 	
 */

#include <kernel/system.h>
#include <kernel/vm.h>

#if USE_UMAP

/*==========================================================================*
 *				do_umap					    *
 *==========================================================================*/
int do_umap(m_ptr)
register kipc_msg_t *m_ptr;	/* pointer to request message */
{
/* Map virtual address to physical, for non-kernel processes. */
  int seg_type = m_ptr->CP_SRC_SPACE & SEGMENT_TYPE;
  int seg_index = m_ptr->CP_SRC_SPACE & SEGMENT_INDEX;
  vir_bytes offset = m_ptr->CP_SRC_ADDR;
  int count = m_ptr->CP_NR_BYTES;
  int endpt = (int) m_ptr->CP_SRC_ENDPT;
  int proc_nr, r;
  int naughty = 0;
  phys_bytes phys_addr = 0, lin_addr = 0;
  int caller_pn;
  struct proc *targetpr, *caller;

  /* Verify process number. */
  if (endpt == ENDPT_SELF)
	proc_nr = who_p;
  else
	if (! isokendpt(endpt, &proc_nr))
		return(-EINVAL);
  targetpr = proc_addr(proc_nr);

  okendpt(who_e, &caller_pn);
  caller = proc_addr(caller_pn);

  /* See which mapping should be made. */
  switch(seg_type) {
  case LOCAL_SEG:
      phys_addr = lin_addr = umap_local(targetpr, seg_index, offset, count); 
      if(!lin_addr) return -EFAULT;
      naughty = 1;
      break;
  case REMOTE_SEG:
      phys_addr = lin_addr = umap_remote(targetpr, seg_index, offset, count); 
      if(!lin_addr) return -EFAULT;
      naughty = 1;
      break;
  case LOCAL_VM_SEG:
    if(seg_index == MEM_GRANT) {
	vir_bytes newoffset;
	endpoint_t newep;
	int new_proc_nr;

        if(verify_grant(targetpr->p_endpoint, ENDPT_ANY, offset, count, 0, 0,
                &newoffset, &newep) != 0) {
                kprintf("SYSTEM: do_umap: verify_grant in %s, grant %d, bytes 0x%lx, failed, caller %s\n", targetpr->p_name, offset, count, caller->p_name);
		proc_stacktrace(caller);
                return -EFAULT;
        }

        if(!isokendpt(newep, &new_proc_nr)) {
                kprintf("SYSTEM: do_umap: isokendpt failed\n");
                return -EFAULT;
        }

	/* New lookup. */
	offset = newoffset;
	targetpr = proc_addr(new_proc_nr);
	seg_index = D;
      }

      if(seg_index == T || seg_index == D || seg_index == S) {
        phys_addr = lin_addr = umap_local(targetpr, seg_index, offset, count); 
      } else {
	kprintf("SYSTEM: bogus seg type 0x%lx\n", seg_index);
	return -EFAULT;
      }
      if(!lin_addr) {
	kprintf("SYSTEM:do_umap: umap_local failed\n");
	return -EFAULT;
      }
      if(vm_lookup(targetpr, lin_addr, &phys_addr, NULL) != 0) {
	kprintf("SYSTEM:do_umap: vm_lookup failed\n");
	return -EFAULT;
      }
      if(phys_addr == 0)
	minix_panic("vm_lookup returned zero physical address", NO_NUM);
      break;
  default:
      if((r=arch_umap(targetpr, offset, count, seg_type, &lin_addr))
	!= 0)
	return r;
      phys_addr = lin_addr;
  }

  if(vm_running && !vm_contiguous(targetpr, lin_addr, count)) {
	kprintf("SYSTEM:do_umap: not contiguous\n");
	return -EFAULT;
  }

  m_ptr->CP_DST_ADDR = phys_addr;
  if(naughty || phys_addr == 0) {
	  kprintf("kernel: umap 0x%x done by %d / %s, pc 0x%lx, 0x%lx -> 0x%lx\n",
		seg_type, who_e, caller->p_name, caller->p_reg.pc, offset, phys_addr);
	kprintf("caller stack: ");
	proc_stacktrace(caller);
  }
  return (phys_addr == 0) ? -EFAULT: 0;
}

#endif /* USE_UMAP */
