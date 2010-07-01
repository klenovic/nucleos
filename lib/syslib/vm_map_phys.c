/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/lib.h>
#include <nucleos/mman.h>
#include <nucleos/syslib.h>
#include <nucleos/vm.h>
#include <stdarg.h>
#include <nucleos/string.h>
#include <nucleos/errno.h>

void *vm_map_phys(endpoint_t who, void *phaddr, size_t len)
{
	kipc_msg_t m;
	int r;

	m.VMMP_EP = who;
	m.VMMP_PHADDR = phaddr;
	m.VMMP_LEN = len;

	r = ktaskcall(VM_PROC_NR, VM_MAP_PHYS, &m);

	if(r != 0) return MAP_FAILED;

	return (void *) m.VMMP_VADDR_REPLY;
}

int vm_unmap_phys(endpoint_t who, void *vaddr, size_t len)
{
	kipc_msg_t m;
	int r;

	m.VMUP_EP = who;
	m.VMUP_VADDR = vaddr;

	return ktaskcall(VM_PROC_NR, VM_UNMAP_PHYS, &m);
}

