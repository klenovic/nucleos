/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The kernel call that is implemented in this file:
 *   m_type:    SYS_PROFBUF
 *
 * The parameters for this kernel call are:
 *    m_data5:    PROF_CTL_PTR      (location of control struct)
 *    m_data6:    PROF_MEM_PTR      (location of profiling table)
 *
 * Changes:
 *   14 Aug, 2006   Created (Rogier Meurs)
 */

#include <kernel/system.h>

/*===========================================================================*
 *				do_profbuf				     *
 *===========================================================================*/
int do_profbuf(m_ptr)
register kipc_msg_t *m_ptr;    /* pointer to request message */
{
/* This kernel call is used by profiled system processes when Call
 * Profiling is enabled. It is called on the first execution of procentry.
 * By means of this kernel call, the profiled processes inform the kernel
 * about the location of their profiling table and the control structure
 * which is used to enable the kernel to have the tables cleared.
 */ 
  int proc_nr;
  vir_bytes vir_dst;
  struct proc *rp;                          

  /* Store process name, control struct, table locations. */
  if(!isokendpt(m_ptr->m_source, &proc_nr))
	return -EDEADSRCDST;

  if(cprof_procs_no >= NR_SYS_PROCS)
	return -ENOSPC;

  rp = proc_addr(proc_nr);

  cprof_proc_info[cprof_procs_no].endpt = who_e;
  cprof_proc_info[cprof_procs_no].name = rp->p_name;

  cprof_proc_info[cprof_procs_no].ctl_v = (vir_bytes) m_ptr->PROF_CTL_PTR;
  cprof_proc_info[cprof_procs_no].buf_v = (vir_bytes) m_ptr->PROF_MEM_PTR;

  cprof_procs_no++;

  return 0;
}


