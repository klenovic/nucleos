/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/errno.h>
#include <stdio.h>
#include <nucleos/ioctl.h>
#include <nucleos/socket.h>

#include <net/in.h>
#include <net/tcp.h>
#include <net/tcp_io.h>

#include <asm/ioctls.h>

#define DEBUG 1

static int _tcp_shutdown(int socket, int how);

int shutdown(int socket, int how)
{
	int r;
	nwio_tcpconf_t tcpconf;

	r= ioctl(socket, NWIOGTCPCONF, &tcpconf);
	if (r != -1 || errno != ENOTTY)
	{
		if (r == -1)
		{
			/* Bad file descriptor */
			return -1;
		}
		return _tcp_shutdown(socket, how);
	}
#if DEBUG
	fprintf(stderr, "shutdown: not implemented for fd %d\n", socket);
#endif
	errno= ENOSYS;
	return -1;
}

static int _tcp_shutdown(int socket, int how)
{
	int r;

	if (how == SHUT_WR || how == SHUT_RDWR)
	{
		r= ioctl(socket, NWIOTCPSHUTDOWN, NULL);
		if (r == -1)
			return -1;
		if (how == SHUT_WR)
			return 0;
	}

	/* We can't shutdown the read side of the socket. */
	errno= ENOSYS;
	return -1;
}


