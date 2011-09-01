#ifndef _NUCLEOS_CACHE_H
#define _NUCLEOS_CACHE_H

#include <nucleos/kernel.h>
#include <asm/cache.h>

#ifndef L1_CACHE_ALIGN
#define L1_CACHE_ALIGN(x) ALIGN(x, L1_CACHE_BYTES)
#endif

#ifndef SMP_CACHE_BYTES
#define SMP_CACHE_BYTES L1_CACHE_BYTES
#endif

#endif /* _NUCLEOS_CACHE_H */
