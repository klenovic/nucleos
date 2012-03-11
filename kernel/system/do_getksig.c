/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The kernel call that is implemented in this file:
 *   m_type:	SYS_GETKSIG
 *
 * The parameters for this kernel call are:
 *     m_data1:	SIG_ENDPT  	# process with pending signals
 *     m_data4:	SIG_MAP		# bit map with pending signals
 */

#include <kernel/system.h>
#include <nucleos/signal.h>
#include <asm/sigcontext.h>
#include <nucleos/endpoint.h>

#if USE_GETKSIG

/*===========================================================================*
 *			      do_getksig				     *
 *===========================================================================*/
int do_getksig(m_ptr)
kipc_msg_t *m_ptr;			/* pointer to request message */
{
/* PM is ready to accept signals and repeatedly does a kernel call to get 
 * one. Find a process with pending signals. If no signals are available, 
 * return ENDPT_NONE in the process number field.
 * It is not sufficient to ready the process when PM is informed, because 
 * PM can block waiting for VFS_PROC_NR to do a core dump.
 */
  register struct proc *rp;

  /* Find the next process with pending signals. */
  for (rp = BEG_USER_ADDR; rp < END_PROC_ADDR; rp++) {
      if (RTS_ISSET(rp, RTS_SIGNALED)) {
	  /* store signaled process' endpoint */
          m_ptr->SIG_ENDPT = rp->p_endpoint;
          m_ptr->SIG_MAP = rp->p_pending;	/* pending signals map */
          sigemptyset(&rp->p_pending); 		/* ball is in PM's court */
	  RTS_LOCK_UNSET(rp, RTS_SIGNALED);		/* blocked by SIG_PENDING */
          return(0);
      }
  }

  /* No process with pending signals was found. */
  m_ptr->SIG_ENDPT = ENDPT_NONE; 
  return(0);
}
#endif /* USE_GETKSIG */

