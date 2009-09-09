/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*	tcflush() - flush buffered characters		Author: Kees J. Bot
 *								13 Jan 1994
 */
#include <nucleos/termios.h>
#include <nucleos/ioctl.h>
#include <asm/ioctls.h>

int tcflush(int fd, int queue_selector)
{
  return(ioctl(fd, TCFLSH, &queue_selector));
}
