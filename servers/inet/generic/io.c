/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
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

void writeIpAddr(addr)
ipaddr_t addr;
{
#define addrInBytes ((u8_t *)&addr)

	printk("%d.%d.%d.%d", addrInBytes[0], addrInBytes[1],
		addrInBytes[2], addrInBytes[3]);
#undef addrInBytes
}

void writeEtherAddr(addr)
ether_addr_t *addr;
{
#define addrInBytes ((u8_t *)addr->ea_addr)

	printk("%x:%x:%x:%x:%x:%x", addrInBytes[0], addrInBytes[1],
		addrInBytes[2], addrInBytes[3], addrInBytes[4], addrInBytes[5]);
#undef addrInBytes
}
