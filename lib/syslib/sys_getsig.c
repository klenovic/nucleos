/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include "syslib.h"

/*===========================================================================*
 *                                sys_getksig				     *
 *===========================================================================*/
PUBLIC int sys_getksig(k_proc_nr, k_sig_map)
int *k_proc_nr;				/* return process number here */
sigset_t *k_sig_map;			/* return signal map here */
{
    message m;
    int result;

    result = _taskcall(SYSTASK, SYS_GETKSIG, &m);
    *k_proc_nr = m.SIG_ENDPT;
    *k_sig_map = (sigset_t) m.SIG_MAP;
    return(result);
}

