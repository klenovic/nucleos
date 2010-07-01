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
server/ip/gen/tcp.h
*/

#ifndef __SERVER__IP__GEN__TCP_H__
#define __SERVER__IP__GEN__TCP_H__

#define TCP_MIN_HDR_SIZE	20
#define TCP_MAX_HDR_SIZE	60

#define TCPPORT_TELNET		23
#define TCPPORT_FINGER		79

#define TCPPORT_RESERVED	1024

typedef u16_t tcpport_t;

#endif /* __SERVER__IP__GEN__TCP_H__ */
