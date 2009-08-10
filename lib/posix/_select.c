/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

#define select _select

#include <lib.h>
#include <nucleos/time.h>

int select(int nfds,
	fd_set *readfds, fd_set *writefds, fd_set *errorfds,
	struct timeval *timeout)
{
  message m;

  m.SEL_NFDS = nfds;
  m.SEL_READFDS = (char *) readfds;
  m.SEL_WRITEFDS = (char *) writefds;
  m.SEL_ERRORFDS = (char *) errorfds;
  m.SEL_TIMEOUT = (char *) timeout;

  return (_syscall(FS, SELECT, &m));
}

