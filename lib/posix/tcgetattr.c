/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/ioctl.h>
#include <nucleos/errno.h>
#include <nucleos/termios.h>
#include <asm/ioctls.h>

int tcgetattr(int fd, struct termios *termios_p)
{
  return(ioctl(fd, TCGETS, termios_p));
}
