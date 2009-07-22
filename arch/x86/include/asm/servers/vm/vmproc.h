/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __X86_ASM_SERVERS_VM_VMPROC_H
#define __X86_ASM_SERVERS_VM_VMPROC_H

#include <asm/kernel/types.h>

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

#endif /* __X86_ASM_SERVER_VM_VMPROC_H */
