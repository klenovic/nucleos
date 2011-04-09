/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The kernel call that is implemented in this file:
 *   m_type:	SYS_ENDKSIG
 *
 * The parameters for this kernel call are:
 *     m_data1:	SIG_ENDPT  	# process for which PM is done
 */

#include <kernel/system.h>
#include <nucleos/signal.h>
#include <asm/sigcontext.h>

#if USE_ENDKSIG 

/*===========================================================================*
 *			      do_endksig				     *
 *===========================================================================*/
int do_endksig(m_ptr)
kipc_msg_t *m_ptr;			/* pointer to request message */
{
/* Finish up after a kernel type signal, caused by a SYS_KILL message or a 
 * call to cause_sig by a task. This is called by the PM after processing a
 * signal it got with SYS_GETKSIG.
 */
  register struct proc *rp;
  int proc_nr;

  /* Get process pointer and verify that it had signals pending. If the 
   * process is already dead its flags will be reset. 
   */
  if(!isokendpt(m_ptr->SIG_ENDPT, &proc_nr))
	return -EINVAL;

  rp = proc_addr(proc_nr);
  if (!RTS_ISSET(rp, RTS_SIG_PENDING)) return(-EINVAL);

  /* PM has finished one kernel signal. Perhaps process is ready now? */
  if (!RTS_ISSET(rp, RTS_SIGNALED)) 		/* new signal arrived */
	RTS_LOCK_UNSET(rp, RTS_SIG_PENDING);	/* remove pending flag */
  return(0);
}

#endif /* USE_ENDKSIG */

