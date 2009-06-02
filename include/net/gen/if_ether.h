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
server/ip/gen/if_ether.h
*/

#ifndef __SERVER__IP__GEN__IF_ETHER_H__
#define __SERVER__IP__GEN__IF_ETHER_H__

struct ether_addr;

#define _PATH_ETHERS	"/etc/ethers"

char *ether_ntoa _ARGS(( struct ether_addr *e ));
struct ether_addr *ether_aton _ARGS(( const char *s ));
int ether_ntohost _ARGS(( char *hostname, struct ether_addr *e ));
int ether_hostton _ARGS(( char *hostname, struct ether_addr *e ));
int ether_line _ARGS(( char *l, struct ether_addr *e, char *hostname ));

#endif /* __SERVER__IP__GEN__IF_ETHER_H__ */
