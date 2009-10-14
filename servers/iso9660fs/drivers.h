#ifndef __SERVERS_ISO9660FS_DRIVERS_H
#define __SERVERS_ISO9660FS_DRIVERS_H

#include <nucleos//dmap.h>

/* Driver endpoints for major devices. Only the block devices
 * are mapped here, it's a subset of the mapping in the VFS */

struct driver_endpoints {
	endpoint_t driver_e;
};

extern struct driver_endpoints driver_endpoints[];

#endif /* __SERVERS_ISO9660FS_DRIVERS_H */
