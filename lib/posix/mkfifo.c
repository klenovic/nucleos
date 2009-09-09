/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/lib.h>
#include <nucleos/stat.h>
#include <nucleos/unistd.h>

int mkfifo(const char *name, mode_t mode)
{
  return mknod(name, mode | S_IFIFO, (dev_t) 0);
}
