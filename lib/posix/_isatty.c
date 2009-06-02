/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <lib.h>
#define isatty _isatty
#define tcgetattr _tcgetattr
#include <termios.h>
#include <unistd.h>

PUBLIC int isatty(fd)
int fd;
{
  struct termios dummy;

  return(tcgetattr(fd, &dummy) == 0);
}
