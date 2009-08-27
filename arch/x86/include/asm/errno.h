#ifndef __ASM_X86_ERRNO_H
#define __ASM_X86_ERRNO_H

#include <asm-generic/errno.h>

/* Don't show outside the kernel/ukernel */
#if defined(__KERNEL__) || defined(__UKERNEL__)
extern int errno;
#endif

#endif /* __ASM_X86_ERRNO_H */
