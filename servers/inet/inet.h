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
inet/inet.h

Created:	Dec 30, 1991 by Philip Homburg

Copyright 1995 Philip Homburg
*/

#ifndef INET__INET_H
#define INET__INET_H

#include <nucleos/types.h>
#include <nucleos/errno.h>
#include <nucleos/stddef.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/ioc_file.h>
#include <nucleos/time.h>
#include <nucleos/type.h>

typedef int ioreq_t;

#include <nucleos/const.h>
#include <nucleos/com.h>
#include <nucleos/syslib.h>
#include <nucleos/sysutil.h>
#include <net/hton.h>
#include <net/gen/ether.h>
#include <net/gen/eth_hdr.h>
#include <net/gen/eth_io.h>
#include <net/gen/in.h>
#include <net/gen/ip_hdr.h>
#include <net/gen/ip_io.h>
#include <net/gen/icmp.h>
#include <net/gen/icmp_hdr.h>
#include <net/gen/oneCsum.h>
#include <net/gen/psip_hdr.h>
#include <net/gen/psip_io.h>
#include <net/gen/route.h>
#include <net/gen/tcp.h>
#include <net/gen/tcp.h>
#include <net/gen/tcp_hdr.h>
#include <net/gen/tcp_io.h>
#include <net/gen/udp.h>
#include <net/gen/udp_hdr.h>
#include <net/gen/udp_io.h>

#include <net/gen/arp_io.h>
#include <net/ioctl.h>

#include "const.h"
#include "inet_config.h"

void panic0(char *file, int line);
void inet_panic(void) __noreturn; 

#define ip_panic(print_list)  \
	(panic0(__FILE__, __LINE__), printf print_list, panic())
#define panic() inet_panic()

#if DEBUG
#define ip_warning(print_list)  \
	( \
		printf("warning at %s, %d: ", __FILE__, __LINE__), \
		printf print_list, \
		printf("\ninet stacktrace: "), \
		util_stacktrace() \
	)
#else
#define ip_warning(print_list)	((void) 0)
#endif

#define DBLOCK(level, code) \
	do { if ((level) & DEBUG) { where(); code; } } while(0)
#define DIFBLOCK(level, condition, code) \
	do { if (((level) & DEBUG) && (condition)) \
		{ where(); code; } } while(0)

extern int this_proc;
extern char version[];

#ifndef HZ
extern u32_t system_hz;
#define HZ system_hz
#define HZ_DYNAMIC 1
#endif

#endif /* INET__INET_H */
