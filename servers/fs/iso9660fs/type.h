#ifndef __SERVERS_ISO9660FS_TYPE_H
#define __SERVERS_ISO9660FS_TYPE_H

#include <nucleos/types.h>

typedef __u32 __kernel_block_t;	/* block number of its (minor) device */

#if defined(__KERNEL__) || defined(__UKERNEL__)
typedef __kernel_block_t block_t;	/* block number of its (minor) device */
#endif

#endif /* __SERVERS_ISO9660FS_TYPE_H */
