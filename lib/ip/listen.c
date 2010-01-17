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
#include <nucleos/ioctl.h>
#include <nucleos/socket.h>
#include <netinet/in.h>

#include <net/in.h>
#include <net/tcp.h>
#include <net/tcp_io.h>
#include <net/udp.h>
#include <net/udp_io.h>

#include <asm/ioctls.h>

#define DEBUG 0

int listen(int socket, int backlog)
{
	int r;

	r= ioctl(socket, NWIOTCPLISTENQ, &backlog);
	if (r != -1 || errno != EBADIOCTL)
		return r;

#if DEBUG
	fprintf(stderr, "listen: not implemented for fd %d\n", socket);
#endif
	errno= ENOSYS;
	return -1;
}

