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
 *                                vm_exit				     *
 *===========================================================================*/
int vm_exit(endpoint_t ep)
{
    kipc_msg_t m;
    int result;

    m.VME_ENDPOINT = ep;

    result = ktaskcall(VM_PROC_NR, VM_EXIT, &m);
    return(result);
}


/*===========================================================================*
 *                                vm_willexit				     *
 *===========================================================================*/
int vm_willexit(endpoint_t ep)
{
    kipc_msg_t m;
    int result;

    m.VMWE_ENDPOINT = ep;

    result = ktaskcall(VM_PROC_NR, VM_WILLEXIT, &m);
    return(result);
}

