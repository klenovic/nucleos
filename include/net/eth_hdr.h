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
server/ip/gen/eth_hdr.h
*/

#ifndef __SERVER__IP__GEN__ETH_HDR_H__
#define __SERVER__IP__GEN__ETH_HDR_H__

typedef struct eth_hdr
{
	ether_addr_t eh_dst;
	ether_addr_t eh_src;
	ether_type_t eh_proto;
} eth_hdr_t;

#endif /* __SERVER__IP__GEN__ETH_HDR_H__ */
