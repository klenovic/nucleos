/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The kernel call that is implemented in this file:
 *   m_type:	SYS_SIGRETURN
 *
 * The parameters for this kernel call are:
 *     m_data1:	SIG_ENDPT  	# process returning from handler
 *     m_data6:	SIG_CTXT_PTR 	# pointer to sigcontext structure
 *
 */

#include <kernel/system.h>
#include <nucleos/string.h>
#include <nucleos/signal.h>
#include <ibm/cpu.h>
#include <asm/sigcontext.h>

#if USE_SIGRETURN 

/*===========================================================================*
 *			      do_sigreturn				     *
 *===========================================================================*/
int do_sigreturn(m_ptr)
kipc_msg_t *m_ptr;			/* pointer to request message */
{
/* POSIX style signals require sys_sigreturn to put things in order before 
 * the signalled process can resume execution
 */
  struct sigcontext sc;
  register struct proc *rp;
  int proc_nr, r;

  if (! isokendpt(m_ptr->SIG_ENDPT, &proc_nr)) return(-EINVAL);
  if (iskerneln(proc_nr)) return(-EPERM);
  rp = proc_addr(proc_nr);

  /* Copy in the sigcontext structure. */
  if((r=data_copy(m_ptr->SIG_ENDPT, (vir_bytes) m_ptr->SIG_CTXT_PTR,
	SYSTEM, (vir_bytes) &sc, sizeof(struct sigcontext))) != 0)
	return r;

  /* Restore user bits of psw from sc, maintain system bits from proc. */
  sc.sc_psw  =  (sc.sc_psw & X86_FLAGS_USER) |
                (rp->p_reg.psw & ~X86_FLAGS_USER);

#ifdef CONFIG_X86_32
  /* Don't panic kernel if user gave bad selectors. */
  sc.sc_cs = rp->p_reg.cs;
  sc.sc_ds = rp->p_reg.ds;
  sc.sc_es = rp->p_reg.es;
  sc.sc_ss = rp->p_reg.ss;
  sc.sc_fs = rp->p_reg.fs;
  sc.sc_gs = rp->p_reg.gs;
#endif /* CONFIG_X86_32 */

  /* Restore the registers. */
  memcpy(&rp->p_reg, &sc.sc_regs, sizeof(sigregs));
#ifdef CONFIG_X86_32
  if(sc.sc_flags & MF_FPU_INITIALIZED)
  {
	memcpy(rp->p_fpu_state.fpu_save_area_p, &sc.sc_fpu_state, FPU_XFP_SIZE);
	rp->p_misc_flags |=  MF_FPU_INITIALIZED; /* Restore math usage flag. */
  }
#endif

  return 0;
}
#endif /* USE_SIGRETURN */

