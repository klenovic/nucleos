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
 *                                sys_mapdma				     *
 *===========================================================================*/
PUBLIC int sys_mapdma(vir_addr, bytes)
vir_bytes vir_addr;			/* address in bytes with segment*/
vir_bytes bytes;			/* number of bytes to be copied */
{
    message m;
    int result;

    m.CP_SRC_ADDR = vir_addr;
    m.CP_NR_BYTES = bytes;

    result = _taskcall(SYSTASK, SYS_MAPDMA, &m);
    return(result);
}

