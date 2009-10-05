/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __ASM_GENERIC_ERRNO_H
#define __ASM_GENERIC_ERRNO_H

#include <asm-generic/errno-base.h>

#define EDEADLK			35  /* resource deadlock avoided */
#define ENAMETOOLONG		36  /* file name too long */
#define ENOLCK			37  /* no locks available */
#define ENOSYS			38  /* function not implemented */
#define ENOTEMPTY		39  /* directory not empty */
#define ELOOP			40  /* too many levels of symlinks detected */

#define ERESTART		41  /* driver restarted */
#define EIDRM			43  /* Identifier removed */

/* The following errors relate to networking. */
#define EPACKSIZE		50  /* invalid packet size for some protocol */
#define EOUTOFBUFS		51  /* not enough buffers left */
#define EBADIOCTL		52  /* illegal ioctl for device */
#define EBADMODE		53  /* badmode in ioctl */
#define EWOULDBLOCK		54  /* call would block on nonblocking socket */
#define EBADDEST		55  /* not a valid destination address */
#define EDSTNOTRCH		56  /* destination not reachable */
#define EISCONN			57  /* already connected */
#define EADDRINUSE		58  /* address in use */
#define ECONNREFUSED		59  /* connection refused */
#define ECONNRESET		60  /* connection reset */
#define ETIMEDOUT		61  /* connection timed out */
#define EURG			62  /* urgent data present */
#define ENOURG			63  /* no urgent data present */
#define ENOTCONN		64  /* no connection (yet or anymore) */
#define ESHUTDOWN		65  /* a write call to a shutdown connection */
#define ENOCONN			66  /* no such connection */
#define EAFNOSUPPORT		67  /* address family not supported */
#define EPROTONOSUPPORT		68  /* protocol not supported by AF */
#define EPROTOTYPE		69  /* Protocol wrong type for socket */
#define EINPROGRESS		70  /* Operation now in progress */
#define EADDRNOTAVAIL		71  /* Can't assign requested address */
#define EALREADY		72  /* Connection already in progress */
#define EMSGSIZE		73  /* Message too long */
#define ENOTSOCK		74  /* Socket operation on non-socket */
#define ENOPROTOOPT		75  /* Protocol not available */
#define EOPNOTSUPP		76  /* Operation not supported */

#endif /* __ASM_GENERIC_ERRNO_H */
