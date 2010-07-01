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
net/netlib.h
*/

#ifndef _NET__NETLIB_H_
#define _NET__NETLIB_H_

int iruserok(unsigned long raddr, int superuser, const char *ruser, const char *luser);
int rcmd(char **ahost, int rport, const char *locuser, const char *remuser, const char *cmd,
	 int *fd2p);

#define IPSTAT_DEVICE	"/dev/ipstat"
#define ETH_DEVICE	"/dev/eth"
#define IP_DEVICE	"/dev/ip"
#define TCP_DEVICE	"/dev/tcp"
#define UDP_DEVICE	"/dev/udp"

#endif /* _NET__NETLIB_H_ */
