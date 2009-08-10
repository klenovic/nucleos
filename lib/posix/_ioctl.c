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
#define ioctl	_ioctl
#include <nucleos/com.h>
#include <sys/ioctl.h>

int ioctl(fd, request, data)
int fd;
int request;
void *data;
{
  message m;

  m.TTY_LINE = fd;
  m.TTY_REQUEST = request;
  m.ADDRESS = (char *) data;
  return(_syscall(FS, IOCTL, &m));
}
