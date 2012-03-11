/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <nucleos/types.h>
#include <nucleos/signal.h>

int kill(int pid, int sig);
pid_t getpid(void);

int raise(int sig)
{
	if (sig < 0 || sig >= _NSIG)
		return -1;
	return kill(getpid(), sig);
}
