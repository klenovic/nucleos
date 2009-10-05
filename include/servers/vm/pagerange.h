#ifndef __SERVERS_VM_PAGERANGE_H
#define __SERVERS_VM_PAGERANGE_H

#include <nucleos/unistd.h>
#include <nucleos/com.h>
#include <nucleos/const.h>
#include <nucleos/endpoint.h>
#include <nucleos/keymap.h>
#include <nucleos/minlib.h>
#include <nucleos/type.h>
#include <nucleos/kipc.h>
#include <nucleos/sysutil.h>
#include <nucleos/syslib.h>
#include <servers/ds/ds.h>

typedef struct pagerange {
	phys_bytes	addr;	/* in pages */
	phys_bytes	size;	/* in pages */

	/* AVL fields */
	struct pagerange *less, *greater;	/* children */
	int		factor;	/* AVL balance factor */
} pagerange_t;

#endif /*  __SERVERS_VM_PAGERANGE_H */
