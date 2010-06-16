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
 *                               sys_enable_iop				     *    
 *===========================================================================*/
int sys_enable_iop(proc_ep)
endpoint_t proc_ep;			/* number of process to allow I/O */
{
    kipc_msg_t m_iop;
    m_iop.IO_ENDPT = proc_ep;
    return ktaskcall(SYSTASK, SYS_IOPENABLE, &m_iop);
}


