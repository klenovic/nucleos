/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/lib.h>
#define munmap	_munmap
#include <nucleos/mman.h>
#include <stdarg.h>
#include <nucleos/string.h>
#include <nucleos/errno.h>

void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset)
{
	message m;
	int r;

	m.VMM_ADDR = (vir_bytes) addr;
	m.VMM_LEN = len;
	m.VMM_PROT = prot;
	m.VMM_FLAGS = flags;
	m.VMM_FD = fd;
	m.VMM_OFFSET = offset;

	r = _syscall(VM_PROC_NR, VM_MMAP, &m);

	if(r != 0) {
		return MAP_FAILED;
	}

	return (void *) m.VMM_RETADDR;
}

int munmap(void *addr, size_t len)
{
	message m;

	m.VMUM_ADDR = addr;
	m.VMUM_LEN = len;

	return _syscall(VM_PROC_NR, VM_UNMAP, &m);
}
