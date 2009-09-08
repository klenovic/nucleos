/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/errno.h>
#include <stdio.h>
#include <nucleos/string.h>
#include <stdlib.h>
#include <nucleos/fcntl.h>
#include <ifaddrs.h>
#include <nucleos/ioctl.h>
#include <nucleos/socket.h>
#include <nucleos/types.h>
#include <netinet/in.h>

#include <net/gen/in.h>
#include <net/gen/ip_io.h>
#include <net/gen/tcp.h>
#include <net/gen/tcp_io.h>
#include <net/gen/udp.h>
#include <net/gen/udp_io.h>

#include <asm/ioctls.h>

int
getifaddrs(struct ifaddrs **ifap)
{
	static int fd = -1;
	nwio_ipconf_t ipconf;
	int flags, err, r;
	static struct ifaddrs ifa;
	static struct sockaddr_in addr, netmask;

	memset(&ifa, 0, sizeof(ifa));
	memset(&addr, 0, sizeof(addr));
	memset(&netmask, 0, sizeof(netmask));
	ifa.ifa_next = NULL;
	ifa.ifa_name = "ip";
	addr.sin_family = netmask.sin_family = AF_INET;
	ifa.ifa_addr = (struct sockaddr *) &addr;
	ifa.ifa_netmask = (struct sockaddr *) &netmask;
	addr.sin_addr.s_addr = 0;
	netmask.sin_addr.s_addr = 0;

	if(fd < 0) {
		char *ipd;
		if(!(ipd=getenv("IP_DEVICE")))
			ipd="/dev/ip";
		if((fd = open(ipd, O_RDWR)) < 0)
			return -1;
	}

	/* Code taken from commands/simple/ifconfig.c. */

	if((flags = fcntl(fd, F_GETFL)) < 0 ||
	   fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0 ||
	   ioctl(fd, NWIOGIPCONF, &ipconf))
		return 0;	/* Report interface as down. */

	addr.sin_addr.s_addr = ipconf.nwic_ipaddr;
	netmask.sin_addr.s_addr = ipconf.nwic_netmask;
	if(addr.sin_addr.s_addr) ifa.ifa_flags = IFF_UP;

	/* Just report on this interface. */

	*ifap = &ifa;

	return 0;
}

void
freeifaddrs(struct ifaddrs *ifp)
{
	/* getifaddrs points to static data, so no need to free. */
	;
}

