/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
io.c

Copyright 1995 Philip Homburg
*/

#include <stdlib.h>

#include "../inet.h"
#include "io.h"

PUBLIC void writeIpAddr(addr)
ipaddr_t addr;
{
#define addrInBytes ((u8_t *)&addr)

	printf("%d.%d.%d.%d", addrInBytes[0], addrInBytes[1],
		addrInBytes[2], addrInBytes[3]);
#undef addrInBytes
}

PUBLIC void writeEtherAddr(addr)
ether_addr_t *addr;
{
#define addrInBytes ((u8_t *)addr->ea_addr)

	printf("%x:%x:%x:%x:%x:%x", addrInBytes[0], addrInBytes[1],
		addrInBytes[2], addrInBytes[3], addrInBytes[4], addrInBytes[5]);
#undef addrInBytes
}

/*
 * $PchId: io.c,v 1.6 1998/10/23 20:24:34 philip Exp $
 */
