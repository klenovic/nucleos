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

#include <nucleos/vm.h>

/*===========================================================================*
 *                                vm_allocmem				     *
 *===========================================================================*/
int vm_allocmem(phys_clicks bytes, phys_clicks *retmembase)
{
    kipc_msg_t m;
    int result;

    m.VMAM_BYTES = (char *) bytes;
    result = ktaskcall(VM_PROC_NR, VM_ALLOCMEM, &m);
    if(result == 0)
	    *retmembase = m.VMAM_MEMBASE;

    return result;
}

