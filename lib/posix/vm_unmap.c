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
#include <nucleos/string.h>
#include <nucleos/errno.h>
#include <stdarg.h>

int vm_unmap(int endpt, void *addr)
{
	kipc_msg_t m;

	m.VMUN_ENDPT = endpt;
	m.VMUN_ADDR = (long) addr;

	return ktaskcall(VM_PROC_NR, VM_SHM_UNMAP, &m);
}
