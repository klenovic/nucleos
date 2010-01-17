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
net/arp_io.h

Created:	Jan 2001 by Philip Homburg <philip@f-mnx.phicoh.com>
*/

typedef struct nwio_arp
{
	int nwa_entno;
	u32_t nwa_flags;
	ipaddr_t nwa_ipaddr;
	ether_addr_t nwa_ethaddr;
} nwio_arp_t;

#define NWAF_EMPTY	0
#define NWAF_INCOMPLETE	1
#define NWAF_DEAD	2
#define NWAF_PERM	4
#define NWAF_PUB	8
