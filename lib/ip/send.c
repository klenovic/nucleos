/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <stdlib.h>
#include <nucleos/socket.h>

ssize_t send(int socket, const void *buffer, size_t length, int flags)
{
	return sendto(socket, buffer, length, flags, NULL, 0);
}

