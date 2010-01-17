/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __NUCLEOS_SOCKET_H
#define __NUCLEOS_SOCKET_H

/* Can we include <nucleos/types.h> here or do we need an additional header that is
 * safe to include?
 */
#include <nucleos/types.h>

/* Open Group Base Specifications Issue 6 (not complete) */
#include <net/socket.h>

#define SOCK_STREAM	1
#define SOCK_DGRAM	2
#define SOCK_RAW	3
#define SOCK_RDM	4
#define SOCK_SEQPACKET	5

#define SOL_SOCKET	0xFFFF

#define SO_DEBUG	0x0001
#define SO_REUSEADDR	0x0004
#define SO_KEEPALIVE	0x0008

#define SO_SNDBUF	0x1001	/* send buffer size */
#define SO_RCVBUF	0x1002	/* receive buffer size */
#define SO_ERROR	0x1007	/* get and clear error status */

/* The how argument to shutdown */
#define SHUT_RD		0	/* No further reads */
#define SHUT_WR		1	/* No further writes */
#define SHUT_RDWR	2	/* No further reads and writes */

typedef uint8_t sa_family_t;

typedef int32_t socklen_t;

struct sockaddr
{
	sa_family_t sa_family;
	char sa_data[8];	/* Big enough for sockaddr_in */
};

#if defined(__KERNEL__) || defined(__UKERNEL__)

int accept(int _socket, struct sockaddr *__restrict _address, socklen_t *__restrict _address_len);
int bind(int _socket, const struct sockaddr *_address, socklen_t _address_len);
int connect(int _socket, const struct sockaddr *_address, socklen_t _address_len);
int getpeername(int _socket, struct sockaddr *__restrict _address,
		socklen_t *__restrict _address_len);
int getsockname(int _socket, struct sockaddr *__restrict _address,
		socklen_t *__restrict _address_len);
int setsockopt(int _socket, int _level, int _option_name, const void *_option_value,
	       socklen_t _option_len);
int getsockopt(int _socket, int _level, int _option_name, void *__restrict _option_value,
	       socklen_t *__restrict _option_len);
int listen(int _socket, int _backlog);
ssize_t recv(int _socket, void *_buffer, size_t _length, int _flags);
ssize_t recvfrom(int _socket, void *__restrict _buffer, size_t _length, int _flags,
		 struct sockaddr *__restrict _address, socklen_t *__restrict _address_len);
ssize_t send(int _socket, const void *_buffer, size_t _length, int _flags);
ssize_t sendto(int _socket, const void *_message, size_t _length, int _flags,
	       const struct sockaddr *_dest_addr, socklen_t _dest_len);
int shutdown(int _socket, int _how);
int socket(int _domain, int _type, int _protocol);

#endif /* defined(__KERNEL__) || defined(__UKERNEL__) */

/* The following constants are often used in applications, but are not defined
 * by POSIX.
 */
#define PF_UNIX	AF_UNIX
#define PF_INET	AF_INET

#endif /* __NUCLEOS_SOCKET_H */
