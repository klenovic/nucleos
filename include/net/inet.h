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
server/ip/gen/inet.h
*/

#ifndef __SERVER__IP__GEN__INET_H__
#define __SERVER__IP__GEN__INET_H__

ipaddr_t inet_addr(const char *addr);
ipaddr_t inet_network(const char *addr);
char *inet_ntoa(ipaddr_t addr);
int inet_aton(const char *cp, ipaddr_t *pin);

#endif /* __SERVER__IP__GEN__INET_H__ */
