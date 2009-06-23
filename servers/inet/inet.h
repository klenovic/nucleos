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

#define _SYSTEM	1	/* get OK and negative error codes */

#include <sys/types.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#ifdef __minix_vmd

#include <minix/ansi.h>
#include <minix/cfg_public.h>
#include <nucleos/type.h>

#else /* Assume at least Minix 3.x */

#include <unistd.h>
#include <sys/ioc_file.h>
#include <sys/time.h>
#include <nucleos/type.h>

#define _NORETURN	/* Should be non empty for GCC */

typedef int ioreq_t;

#endif

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

#define PUBLIC
#define EXTERN	extern
#define PRIVATE	static
#define FORWARD	static

#define THIS_FILE static char *this_file= __FILE__;

_PROTOTYPE( void panic0, (char *file, int line) );
_PROTOTYPE( void inet_panic, (void) ) _NORETURN; 

#define ip_panic(print_list)  \
	(panic0(this_file, __LINE__), printf print_list, panic())
#define panic() inet_panic()

#if DEBUG
#define ip_warning(print_list)  \
	( \
		printf("warning at %s, %d: ", this_file, __LINE__), \
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

#if _ANSI
#define ARGS(x) x
#else /* _ANSI */
#define ARGS(x) ()
#endif /* _ANSI */

extern int this_proc;
extern char version[];

#ifndef HZ
EXTERN u32_t system_hz;
#define HZ system_hz
#define HZ_DYNAMIC 1
#endif

#endif /* INET__INET_H */

/*
 * $PchId: inet.h,v 1.16 2005/06/28 14:27:54 philip Exp $
 */
