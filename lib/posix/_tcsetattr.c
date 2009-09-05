/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
posix/_tcsetattr.c

Created:	June 11, 1993 by Philip Homburg
*/

#define tcsetattr _tcsetattr
#define ioctl _ioctl
#include <nucleos/errno.h>
#include <termios.h>
#include <nucleos/ioctl.h>
#include <nucleos/types.h>
#include <asm/ioctls.h>

int tcsetattr(fd, opt_actions, termios_p)
int fd;
int opt_actions;
const struct termios *termios_p;
{
  int request;

  switch(opt_actions)
  {
    case TCSANOW:	request = TCSETS;	break;
    case TCSADRAIN:	request = TCSETSW;	break;
    case TCSAFLUSH:	request = TCSETSF;	break;
    default:		errno = EINVAL;		return(-1);
  };
  return(ioctl(fd, request, (void *) termios_p));
}
