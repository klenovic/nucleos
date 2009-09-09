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
#include <nucleos/fcntl.h>
#include <nucleos/unistd.h>

int dup2(int fd, int fd2)
{
/* The behavior of dup2 is defined by POSIX in 6.2.1.2 as almost, but not
 * quite the same as fcntl.
 */

  if (fd2 < 0 || fd2 > OPEN_MAX) {
	errno = EBADF;
	return(-1);
  }

  /* Check to see if fildes is valid. */
  if (fcntl(fd, F_GETFL) < 0) {
	/* 'fd' is not valid. */
	return(-1);
  } else {
	/* 'fd' is valid. */
	if (fd == fd2) return(fd2);
	close(fd2);
	return(fcntl(fd, F_DUPFD, fd2));
  }
}