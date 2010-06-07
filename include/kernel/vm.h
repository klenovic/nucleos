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

/* Pseudo error codes */
#define VMSUSPEND       -996
#define EFAULT_SRC     -995
#define EFAULT_DST     -994

#define FIXLINMSG(prp) {									\
		if (prp->syscall_0x80)								\
			prp->p_delivermsg_lin = vir2phys(&prp->p_delivermsg);			\
		else										\
			prp->p_delivermsg_lin = umap_local(prp, D, prp->p_delivermsg_vir,	\
							   sizeof(message));			\
	}

#define PHYS_COPY_CATCH(dst, src, size)		\
({						\
	extern int catch_pagefaults;		\
	phys_bytes __pfaddr;			\
	catch_pagefaults++;			\
	__pfaddr = phys_copy(dst, src, size);	\
	catch_pagefaults--;			\
	__pfaddr;				\
})

#endif /* __KERNEL__ */

#endif /* __KERNEL_VM_H */
