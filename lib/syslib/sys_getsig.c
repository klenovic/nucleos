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
 *                                sys_getksig				     *
 *===========================================================================*/
int sys_getksig(proc_ep, k_sig_map)
endpoint_t *proc_ep;			/* return process number here */
sigset_t *k_sig_map;			/* return signal map here */
{
    message m;
    int result;

    result = _taskcall(SYSTASK, SYS_GETKSIG, &m);
    *proc_ep = m.SIG_ENDPT;
    *k_sig_map = (sigset_t) m.SIG_MAP;
    return(result);
}

