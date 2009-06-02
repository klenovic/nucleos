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
#define getuid	_getuid
#include <unistd.h>

PUBLIC uid_t getuid()
{
  message m;

  return( (uid_t) _syscall(MM, GETUID, &m));
}
