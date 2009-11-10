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

	r = ksyscall(VM_PROC_NR, VM_MMAP, &m);

	if(r != 0) {
		return MAP_FAILED;
	}

	return (void *) m.VMM_RETADDR;
}

int __munmap(void *addr, size_t len)
{
	message m;

	m.VMUM_ADDR = addr;
	m.VMUM_LEN = len;

	return ksyscall(VM_PROC_NR, VM_MUNMAP, &m);
}
/* munamp just a weak definition since PM/VM may override it */
int munmap(void *addr, size_t len) __attribute__((weak,alias("__munmap")));

int __munmap_text(void *addr, size_t len)
{
	message m;
	m.VMUM_ADDR = addr;
	m.VMUM_LEN = len;

	return ksyscall(VM_PROC_NR, VM_MUNMAP_TEXT, &m);
}
/* munamp just a weak definition since PM/VM may override it */
int munmap_text(void *addr, size_t len) __attribute__((weak,alias("__munmap_text")));

void *vm_remap(int d, int s, void *da, void *sa, size_t size)
{
	message m;
	int r;

	m.VMRE_D = d;
	m.VMRE_S = s;
	m.VMRE_DA = (char *) da;
	m.VMRE_SA = (char *) sa;
	m.VMRE_SIZE = size;

	r = ksyscall(VM_PROC_NR, VM_REMAP, &m);
	if (r != 0)
		return MAP_FAILED;
	return (void *) m.VMRE_RETA;
}

int vm_unmap(int endpt, void *addr)
{
	message m;

	m.VMUN_ENDPT = endpt;
	m.VMUN_ADDR = (long) addr;

	return ksyscall(VM_PROC_NR, VM_SHM_UNMAP, &m);
}

unsigned long vm_getphys(int endpt, void *addr)
{
	message m;
	unsigned long ret;
	int r;

	m.VMPHYS_ENDPT = endpt;
	m.VMPHYS_ADDR = (long) addr;

	r = ksyscall(VM_PROC_NR, VM_GETPHYS, &m);
	if (r != 0)
		return 0;
	return m.VMPHYS_RETA;
}

u8_t vm_getrefcount(int endpt, void *addr)
{
	message m;
	u8_t ret;
	int r;

	m.VMREFCNT_ENDPT = endpt;
	m.VMREFCNT_ADDR = (long) addr;

	r = ksyscall(VM_PROC_NR, VM_GETREF, &m);
	if (r != 0)
		return (u8_t) -1;
	return (u8_t) m.VMREFCNT_RETC;
}
