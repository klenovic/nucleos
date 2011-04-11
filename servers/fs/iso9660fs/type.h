#ifndef __SERVERS_FS_ISO9660FS_TYPE_H
#define __SERVERS_FS_ISO9660FS_TYPE_H

#include <nucleos/types.h>

#if defined(__KERNEL__) || defined(__UKERNEL__)
typedef u32 block_t;	/* block number of its (minor) device */
#endif

#endif /* __SERVERS_FS_ISO9660FS_TYPE_H */
