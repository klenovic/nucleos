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
posix/_tcflow.c

Created:	June 8, 1993 by Philip Homburg
*/
#include <nucleos/termios.h>
#include <nucleos/ioctl.h>
#include <asm/ioctls.h>

int tcflow(int fd, int action)
{
  return(ioctl(fd, TCFLOW, &action));
}
