/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/types.h>
#include <nucleos/type.h>
#include <nucleos/endpoint.h>
#include <nucleos/syslib.h>

int arch_get_pagefault(endpoint_t *who, vir_bytes *addr, u32 *err)
{
	return sys_vmctl_get_pagefault_i386(who, (u32*)addr, err);
}
