#ifndef __ASM_X86_SIGINFO_H
#define __ASM_X86_SIGINFO_H
/* @nucleos: linux (master: b0483e78e5c) */

#ifdef __x86_64__
# define __ARCH_SI_PREAMBLE_SIZE	(4 * sizeof(int))
#endif

#include <asm-generic/siginfo.h>

#endif /* __ASM_X86_SIGINFO_H */
