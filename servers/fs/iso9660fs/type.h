#ifndef __TYPE_H
#define __TYPE_H

#include <nucleos/types.h>

typedef __u32 __kernel_block_t;	/* block number of its (minor) device */

#if defined(__KERNEL__) || defined(__UKERNEL__)
typedef __kernel_block_t block_t;	/* block number of its (minor) device */
#endif

#endif /* __TYPE_H */
