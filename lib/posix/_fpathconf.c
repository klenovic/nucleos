/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* POSIX fpathconf (Sec. 5.7.1) 		Author: Andy Tanenbaum */

#include <nucleos/lib.h>
#include <nucleos/stat.h>
#include <nucleos/errno.h>
#include <nucleos/limits.h>
#include <nucleos/unistd.h>
#include <nucleos/termios.h>

long fpathconf(fd, name)
int fd;				/* file descriptor being interrogated */
int name;			/* property being inspected */
{
/* POSIX allows some of the values in <nucleos/limits.h> to be increased at
 * run time.  The pathconf and fpathconf functions allow these values
 * to be checked at run time.  MINIX does not use this facility.
 * The run-time limits are those given in <nucleos/limits.h>.
 */

  struct stat stbuf;

  switch(name) {
	case _PC_LINK_MAX:
		/* Fstat the file.  If that fails, return -1. */
		if (fstat(fd, &stbuf) != 0) return(-1);
		if (S_ISDIR(stbuf.st_mode))
			return(1L);	/* no links to directories */
		else
			return( (long) LINK_MAX);

	case _PC_MAX_CANON:
		return( (long) MAX_CANON);

	case _PC_MAX_INPUT:
		return( (long) MAX_INPUT);

	case _PC_NAME_MAX:
		return( (long) NAME_MAX);

	case _PC_PATH_MAX:
		return( (long) PATH_MAX);

	case _PC_PIPE_BUF:
		return( (long) PIPE_BUF);

	case _PC_CHOWN_RESTRICTED:
		return( (long) _POSIX_CHOWN_RESTRICTED);

	case _PC_NO_TRUNC:
		return( (long) _POSIX_NO_TRUNC);

	case _PC_VDISABLE:
		return( (long) _POSIX_VDISABLE);

	default:
		errno = EINVAL;
		return(-1);
  }
}
