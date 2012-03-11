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
 *                                sys_umap				     *
 *===========================================================================*/
int sys_umap(proc_ep, seg, vir_addr, bytes, phys_addr)
endpoint_t proc_ep;			/* process number to do umap for */
int seg;				/* T, D, or S segment */
vir_bytes vir_addr;			/* address in bytes with segment*/
vir_bytes bytes;			/* number of bytes to be copied */
phys_bytes *phys_addr;			/* placeholder for result */
{
    kipc_msg_t m;
    int result;

    m.CP_SRC_ENDPT = proc_ep;
    m.CP_SRC_SPACE = seg;
    m.CP_SRC_ADDR = vir_addr;
    m.CP_NR_BYTES = bytes;

    result = ktaskcall(SYSTASK, SYS_UMAP, &m);
    *phys_addr = m.CP_DST_ADDR;
    return(result);
}

