/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	tcsendbreak() - send a break			Author: Kees J. Bot
 *								13 Jan 1994
 */
#define tcsendbreak _tcsendbreak
#define ioctl _ioctl
#include <termios.h>
#include <nucleos/ioctl.h>
#include <asm/ioctls.h>

int tcsendbreak(int fd, int duration)
{
  return(ioctl(fd, TCSBRK, &duration));
}
