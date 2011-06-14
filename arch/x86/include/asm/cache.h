#ifndef _ASM_X86_CACHE_H
#define _ASM_X86_CACHE_H

#include <nucleos/linkage.h>

/* L1 cache line size */
#define L1_CACHE_SHIFT	(CONFIG_X86_L1_CACHE_SHIFT)
#define L1_CACHE_BYTES	(1 << L1_CACHE_SHIFT)

#endif /* _ASM_X86_CACHE_H */
