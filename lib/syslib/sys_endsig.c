/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/syslib.h>

/*===========================================================================*
 *                                sys_endksig				     *
 *===========================================================================*/
int sys_endksig(proc_ep)
endpoint_t proc_ep;				/* process number */
{
    kipc_msg_t m;
    int result;

    m.SIG_ENDPT = proc_ep;
    result = ktaskcall(SYSTASK, SYS_ENDKSIG, &m);
    return(result);
}

