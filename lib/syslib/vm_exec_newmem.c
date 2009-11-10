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
 *                                vm_exec_newmem			     *
 *===========================================================================*/
int vm_exec_newmem(endpoint_t ep, struct exec_newmem *args,
	int argssize, char **ret_stack_top, int *ret_flags)
{
    message m;
    int result;

    m.VMEN_ENDPOINT = ep;
    m.VMEN_ARGSPTR = (void *) args;
    m.VMEN_ARGSSIZE = argssize;

    result = ktaskcall(VM_PROC_NR, VM_EXEC_NEWMEM, &m);

    *ret_stack_top = m.VMEN_STACK_TOP;
    *ret_flags = m.VMEN_FLAGS;

    return result;
}

