/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
server/ip/gen/psip_io.h
*/

#ifndef __SERVER__IP__GEN__PSIP_IO_H__
#define __SERVER__IP__GEN__PSIP_IO_H__

typedef struct nwio_psipopt
{
	unsigned long nwpo_flags;
} nwio_psipopt_t;

#define NWPO_PROMISC_MASK	0x0001L
#define		NWPO_EN_PROMISC		0x00000001L
#define		NWUO_DI_PROMISC		0x00010000L
#define NWPO_NEXTHOP_MASK	0x0002L
#define		NWPO_EN_NEXTHOP		0x00000002L
#define		NWUO_DI_NEXTHOP		0x00020000L

#endif /* __SERVER__IP__GEN__PSIP_IO_H__ */
