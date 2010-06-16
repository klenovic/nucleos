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

void *vm_remap(int d, int s, void *da, void *sa, size_t size)
{
	kipc_msg_t m;
	int r;

	m.VMRE_D = d;
	m.VMRE_S = s;
	m.VMRE_DA = (char *) da;
	m.VMRE_SA = (char *) sa;
	m.VMRE_SIZE = size;

	r = ktaskcall(VM_PROC_NR, VM_REMAP, &m);
	if (r != 0)
		return MAP_FAILED;
	return (void *) m.VMRE_RETA;
}
