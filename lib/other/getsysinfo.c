/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
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
	message m;
	m.m1_i1 = what;
	m.m1_p1 = where;

	if (ksyscall(who, __NR_getsysinfo, &m) < 0)
		return(-1);

	return(0);
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
	message m;
	m.SIU_WHAT = what;
	m.SIU_WHERE = where;
	m.SIU_LEN = size;

	return ksyscall(who, __NR_getsysinfo_up, &m);
}

