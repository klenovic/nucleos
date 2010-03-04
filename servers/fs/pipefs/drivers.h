
/* Driver endpoints for major devices. Only the block devices
 * are mapped here, it's a subset of the mapping in the VFS */

struct driver_endpoints {
    endpoint_t driver_e;
};

extern struct driver_endpoints driver_endpoints[];


