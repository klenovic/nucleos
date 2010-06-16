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
#include <nucleos/string.h>
#include <nucleos/errno.h>
#include <stdarg.h>

unsigned long vm_getphys(int endpt, void *addr)
{
	kipc_msg_t m;
	unsigned long ret;
	int r;

	m.VMPHYS_ENDPT = endpt;
	m.VMPHYS_ADDR = (long) addr;

	r = ktaskcall(VM_PROC_NR, VM_GETPHYS, &m);
	if (r != 0)
		return 0;
	return m.VMPHYS_RETA;
}
