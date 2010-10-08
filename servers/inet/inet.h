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
#include <nucleos/string.h>
#include <nucleos/unistd.h>
#include <nucleos/time.h>
#include <nucleos/type.h>
#include <asm/ioctls.h>

typedef int ioreq_t;

#include <nucleos/const.h>
#include <nucleos/com.h>
#include <nucleos/syslib.h>
#include <nucleos/sysutil.h>
#include <net/hton.h>
#include <net/ether.h>
#include <net/eth_hdr.h>
#include <net/eth_io.h>
#include <net/in.h>
#include <net/ip_hdr.h>
#include <net/ip_io.h>
#include <net/icmp.h>
#include <net/icmp_hdr.h>
#include <net/oneCsum.h>
#include <net/psip_hdr.h>
#include <net/psip_io.h>
#include <net/route.h>
#include <net/tcp.h>
#include <net/tcp.h>
#include <net/tcp_hdr.h>
#include <net/tcp_io.h>
#include <net/udp.h>
#include <net/udp_hdr.h>
#include <net/udp_io.h>

#include <net/arp_io.h>
#include <asm/ioctls.h>

#include "const.h"
#include "inet_config.h"

void panic0(char *file, int line);
void inet_panic(void) __noreturn; 

#define ip_panic(print_list)  \
	(panic0(__FILE__, __LINE__), printk print_list, panic())
#define panic() inet_panic()

#if DEBUG
#define ip_warning(print_list)  \
	( \
		printk("warning at %s, %d: ", __FILE__, __LINE__), \
		printk print_list, \
		printk("\ninet stacktrace: "), \
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
