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
 *                                vm_umap				     *
 *===========================================================================*/
int vm_umap(int seg, vir_bytes offset, vir_bytes len, phys_bytes *addr)
{
    kipc_msg_t m;
    int result;

    m.VMU_SEG = seg;
    m.VMU_OFFSET = (char *) offset;
    m.VMU_LENGTH = (char *) len;
    result = ktaskcall(VM_PROC_NR, VM_UMAP, &m);
    *addr = (phys_bytes) m.VMU_RETADDR;

    return result;
}

