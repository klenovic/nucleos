/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#if ENABLE_MESSAGE_STATS

#include <lib.h>
#include <unistd.h>

PUBLIC int mstats(struct message_statentry *ms, int entries, int reset)
{
	message m;

	m.m1_i1 = entries;
	m.m1_i2 = reset;
	m.m1_p1 = (void *) ms;

	if(_syscall(MM, MSTATS, &m) < 0) {
		return -1;
	}

	return m.m_type;
}

#endif
