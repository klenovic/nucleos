/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __ASM_X86_SERVERS_VM_VMPROC_H
#define __ASM_X86_SERVERS_VM_VMPROC_H

#include <asm/kernel/types.h>
#include <nucleos/const.h>
#include <nucleos/type.h>
#include <nucleos/com.h>
#include <nucleos/kipc.h>
#include <nucleos/safecopies.h>
#include <nucleos/timer.h>

struct vm_arch {
	struct mem_map	vm_seg[NR_LOCAL_SEGS];	/* text, data, stack */

	/* vm_data_top points to top of data address space, as visible
	 * from user-space, in bytes.
	 * for segments processes this is the same 
	 * as the top of vm_seg[S] segment. for paged processes this
	 * can be much higher (so more memory is available above the
	 * stack).
	 */
	u32_t		vm_data_top;	/* virtual process space in bytes */
};

#endif /* __ASM_X86_SERVER_VM_VMPROC_H */
