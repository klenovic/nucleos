/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
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

u8_t vm_getrefcount(int endpt, void *addr)
{
	kipc_msg_t m;
	u8_t ret;
	int r;

	m.VMREFCNT_ENDPT = endpt;
	m.VMREFCNT_ADDR = (long) addr;

	r = ktaskcall(VM_PROC_NR, VM_GETREF, &m);
	if (r != 0)
		return (u8_t) -1;
	return (u8_t) m.VMREFCNT_RETC;
}
