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
 *                                sys_vm_setbuf				     *
 *===========================================================================*/
PUBLIC int sys_vm_setbuf(base, size, high)
phys_bytes base;
phys_bytes size;
phys_bytes high;
{
    message m;
    int result;

    m.m4_l1= base;
    m.m4_l2= size;
    m.m4_l3= high;

    result = _taskcall(SYSTASK, SYS_VM_SETBUF, &m);
    return(result);
}

