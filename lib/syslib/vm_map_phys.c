/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#define _SYSTEM 1
#include <lib.h>
#include <sys/mman.h>
#include <nucleos/vm.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

PUBLIC void *vm_map_phys(endpoint_t who, void *phaddr, size_t len)
{
	message m;
	int r;

	m.VMMP_EP = who;
	m.VMMP_PHADDR = phaddr;
	m.VMMP_LEN = len;

	r = _taskcall(VM_PROC_NR, VM_MAP_PHYS, &m);

	if(r != OK) return MAP_FAILED;

	return (void *) m.VMMP_VADDR_REPLY;
}

PUBLIC int vm_unmap_phys(endpoint_t who, void *vaddr, size_t len)
{
	message m;
	int r;

	m.VMUP_EP = who;
	m.VMUP_VADDR = vaddr;

	return _taskcall(VM_PROC_NR, VM_UNMAP_PHYS, &m);
}

