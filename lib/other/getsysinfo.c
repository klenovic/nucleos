/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/lib.h>
#include <nucleos/endpoint.h>
#include <nucleos/sysinfo.h>

int getsysinfo(endpoint_t who, int what, void *where)
{
	return INLINE_SYSCALL(getsysinfo, 3, who, what, where);
}

/* Unprivileged variant of getsysinfo. */
/**
 * @brief
 * @param who  from whom to request info
 * @param what  what information is requested
 * @param size  input and output size
 * @param where  where to put it
 */
ssize_t getsysinfo_up(endpoint_t who, int what, size_t size, void *where)
{
	return INLINE_SYSCALL(getsysinfo_up, 4, who, what, where, size);
}
