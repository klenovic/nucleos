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
#define dup	_dup
#define fcntl	_fcntl
#include <fcntl.h>
#include <unistd.h>

int dup(fd)
int fd;
{
  return(fcntl(fd, F_DUPFD, 0));
}
