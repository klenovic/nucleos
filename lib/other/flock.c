/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

/* Library routines
 *
 * Porting to Minix 2.0.0
 * Author:	Giovanni Falzoni <gfalzoni@pointest.com>
 *
 * $Id: flock.c 1291 2005-10-31 14:31:05Z beng $
 */
#include <nucleos/types.h>
#include <nucleos/fcntl.h>
#include <nucleos/string.h>
#include <nucleos/errno.h>
#include <nucleos/unistd.h>

/*
 *	Name:		int flock(int fd, int mode);
 *	Function:	Implements the flock function in Minix.
 */
int flock(int fd, int mode)
{
  struct flock lck;
  register int retcode;

  memset((void *) &lck, 0, sizeof(struct flock));
  lck.l_type = mode & ~LOCK_NB;
  lck.l_pid = getpid();
  if ((retcode = fcntl(fd, mode & LOCK_NB ? F_SETLK : F_SETLKW, &lck)) < 0 && errno == EAGAIN)
	errno = EWOULDBLOCK;
  return retcode;
}

/** flock.c **/
