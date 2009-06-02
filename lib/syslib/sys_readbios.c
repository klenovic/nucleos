/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include "syslib.h"

PUBLIC int sys_readbios(address, buf, size)
phys_bytes address;		/* Absolute memory address */
void *buf;			/* Buffer to store the results */
size_t size;			/* Amount of data to read */
{
/* Read data from BIOS locations */
  message m;

  m.RDB_SIZE = size;
  m.RDB_ADDR = address;
  m.RDB_BUF = buf;
  return(_taskcall(SYSTASK, SYS_READBIOS, &m));
}
