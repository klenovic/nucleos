/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/errno.h>
#include <nucleos/fcntl.h>
#include <stdio.h>
#include <nucleos/unistd.h>
#include <nucleos/string.h>
#include <nucleos/ioctl.h>
#include <nucleos/socket.h>
#include <netinet/in.h>

#include <net/netlib.h>
#include <net/in.h>
#include <net/tcp.h>
#include <net/tcp_io.h>
#include <net/udp.h>
#include <net/udp_io.h>

#include <asm/ioctls.h>

#define DEBUG 0

static int _tcp_accept(int socket, struct sockaddr *__restrict address,
	socklen_t *__restrict address_len);

int accept(int socket, struct sockaddr *__restrict address,
	socklen_t *__restrict address_len)
{
	int r;
	nwio_udpopt_t udpopt;

	r= _tcp_accept(socket, address, address_len);
	if (r != -1 || (errno != ENOTTY && errno != EBADIOCTL))
	return r;

	/* Unfortunately, we have to return EOPNOTSUPP for a socket that
	 * does not support accept (such as a UDP socket) and ENOTSOCK for
	 * filedescriptors that do not refer to a socket.
	 */
	r= ioctl(socket, NWIOGUDPOPT, &udpopt);
	if (r == 0)
	{
		/* UDP socket */
		errno= EOPNOTSUPP;
		return -1;
	}
	if ((errno == ENOTTY || errno == EBADIOCTL))
	{
		errno= ENOTSOCK;
	return -1;
}

	return r;
}

static int _tcp_accept(int socket, struct sockaddr *__restrict address,
	socklen_t *__restrict address_len)
{
	int r, s1, t_errno;
	tcp_cookie_t cookie;

	s1= open(TCP_DEVICE, O_RDWR);
	if (s1 == -1)
		return s1;
	r= ioctl(s1, NWIOGTCPCOOKIE, &cookie);
	if (r == -1)
	{
		t_errno= errno;
		close(s1);
		errno= t_errno;
		return -1;
	}
	r= ioctl(socket, NWIOTCPACCEPTTO, &cookie);
	if (r == -1)
	{
		t_errno= errno;
		close(s1);
		errno= t_errno;
		return -1;
	}
	if (address != NULL)
		getpeername(s1, address, address_len);
	return s1;
}
