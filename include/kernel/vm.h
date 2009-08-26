/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* Prototypes and definitions for VM interface. */
#ifndef __KERNEL_VM_H
#define __KERNEL_VM_H

#ifdef __KERNEL__
#define CHECKRANGE_OR_SUSPEND(pr, start, length, wr)  { int mr; \
	if(vm_running && (mr=vm_checkrange(proc_addr(who_p), pr, start, length, wr, 0)) != 0) { \
		return mr;					 \
	} }

#define CHECKRANGE(pr, start, length, wr)   \
	vm_checkrange(proc_addr(who_p), pr, start, length, wr, 1)

/* Pseudo error code indicating a process request has to be
 * restarted after an OK from VM.
 */
#define VMSUSPEND       -996
#endif /* __KERNEL__ */

#endif /* __KERNEL_VM_H */
