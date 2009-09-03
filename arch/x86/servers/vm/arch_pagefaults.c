/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/callnr.h>
#include <nucleos/com.h>
#include <nucleos/const.h>
#include <servers/ds/ds.h>
#include <nucleos/endpoint.h>
#include <nucleos/keymap.h>
#include <nucleos/minlib.h>
#include <nucleos/type.h>
#include <nucleos/ipc.h>
#include <nucleos/sysutil.h>
#include <nucleos/syslib.h>
#include <nucleos/safecopies.h>

#include <nucleos/errno.h>
#include <string.h>
#include <env.h>
#include <stdio.h>
#include <nucleos/fcntl.h>

#include <servers/vm/glo.h>
#include <servers/vm/proto.h>
#include <servers/vm/util.h>

/*===========================================================================*
 *				arch_handle_pagefaults	     		     *
 *===========================================================================*/
int arch_get_pagefault(who, addr, err)
endpoint_t *who;
vir_bytes *addr;
u32_t *err;
{
	return sys_vmctl_get_pagefault_i386(who, (u32_t*)addr, err);
}
