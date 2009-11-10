/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/syslib.h>

/*===========================================================================*
 *                                sys_sigreturn				     *
 *===========================================================================*/
int sys_sigreturn(proc_ep, sig_ctxt)
endpoint_t proc_ep;			/* for which process */
struct sigmsg *sig_ctxt;		/* POSIX style handling */
{
    message m;
    int result;

    m.SIG_ENDPT = proc_ep;
    m.SIG_CTXT_PTR = (char *) sig_ctxt;
    result = ktaskcall(SYSTASK, SYS_SIGRETURN, &m);
    return(result);
}

