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
 *                                vm_brk				     *
 *===========================================================================*/
int vm_brk(endpoint_t ep, char *addr)
{
    message m;
    int result;

    m.VMB_ENDPOINT = ep;
    m.VMB_ADDR = (void *) addr;

    return ktaskcall(VM_PROC_NR, VM_BRK, &m);
}

