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

#include <nucleos/callnr.h>
#include <nucleos/com.h>
#include <nucleos/const.h>
#include <server/ds/ds.h>
#include <nucleos/endpoint.h>
#include <nucleos/keymap.h>
#include <nucleos/minlib.h>
#include <nucleos/type.h>
#include <nucleos/ipc.h>
#include <nucleos/sysutil.h>
#include <nucleos/syslib.h>
#include <nucleos/safecopies.h>

#include <errno.h>
#include <string.h>
#include <env.h>
#include <stdio.h>
#include <fcntl.h>

#include "../glo.h"
#include "../proto.h"
#include "../util.h"

/*===========================================================================*
 *				arch_handle_pagefaults	     		     *
 *===========================================================================*/
PUBLIC int arch_get_pagefault(who, addr, err)
endpoint_t *who;
vir_bytes *addr;
u32_t *err;
{
	return sys_vmctl_get_pagefault_i386(who, addr, err);
}

