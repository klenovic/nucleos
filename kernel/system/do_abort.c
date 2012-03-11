/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
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
 *    m_data1:	ABRT_HOW 	how to abort
 */
#include <nucleos/kernel.h>
#include <nucleos/unistd.h>
#include <kernel/system.h>

#if USE_ABORT

int do_abort(kipc_msg_t *m_ptr)
{
/* Handle sys_abort. Nucleos is unable to continue. This can originate e.g.
 * in the PM (normal abort or panic) or TTY (after CTRL-ALT-DEL).
 */
	/* Now prepare to shutdown Nucleos. */
	prepare_shutdown(m_ptr->ABRT_HOW);

	return 0;	/* pro-forma (really EDISASTER) */
}

#endif /* USE_ABORT */

