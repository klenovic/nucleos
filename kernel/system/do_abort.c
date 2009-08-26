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
 *   m_type:	SYS_ABORT
 *
 * The parameters for this kernel call are:
 *    m1_i1:	ABRT_HOW 	(how to abort, possibly fetch monitor params)	
 *    m1_i2:	ABRT_MON_PROC 	(proc nr to get monitor params from)	
 *    m1_i3:	ABRT_MON_LEN	(length of monitor params)
 *    m1_p1:	ABRT_MON_ADDR 	(virtual address of params)	
 */
#include <nucleos/nucleos.h>
#include <kernel/system.h>
#include <unistd.h>

#if USE_ABORT

/*===========================================================================*
 *				do_abort				     *
 *===========================================================================*/
int do_abort(m_ptr)
message *m_ptr;			/* pointer to request message */
{
/* Handle sys_abort. MINIX is unable to continue. This can originate e.g.
 * in the PM (normal abort or panic) or TTY (after CTRL-ALT-DEL).
 */
  int how = m_ptr->ABRT_HOW;
  int length;
  phys_bytes src_phys;

  /* See if the monitor is to run the specified instructions. */
  if (how == RBT_MONITOR) {
      int p;
      static char paramsbuffer[512];
      int len;
      len = MIN(m_ptr->ABRT_MON_LEN, sizeof(paramsbuffer)-1);

      if((p=data_copy(m_ptr->ABRT_MON_ENDPT, (vir_bytes) m_ptr->ABRT_MON_ADDR,
		SYSTEM, (vir_bytes) paramsbuffer, len)) != 0) {
		return p;
      }
      paramsbuffer[len] = '\0';

      /* Parameters seem ok, copy them and prepare shutting down. */
      if((p = arch_set_params(paramsbuffer, len+1)) != 0)
	return p;
  }

  /* Now prepare to shutdown MINIX. */
  prepare_shutdown(how);
  return 0;				/* pro-forma (really EDISASTER) */
}

#endif /* USE_ABORT */

