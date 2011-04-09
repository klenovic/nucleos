/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/syslib.h>

/*===========================================================================*
 *                                sys_sigsend				     *
 *===========================================================================*/
int sys_sigsend(proc_ep, sig_ctxt)
endpoint_t proc_ep;			/* for which process */
struct sigmsg *sig_ctxt;		/* POSIX style handling */
{
    kipc_msg_t m;
    int result;

    m.SIG_ENDPT = proc_ep;
    m.SIG_CTXT_PTR = (char *) sig_ctxt;
    result = ktaskcall(SYSTASK, SYS_SIGSEND, &m);
    return(result);
}

