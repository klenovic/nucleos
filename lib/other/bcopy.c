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
/* bcopy - Berklix equivalent of memcpy  */

#include <nucleos/string.h>

void bcopy(src, dst, length)
const void *src;
void *dst;
size_t length;
{
  (void) memcpy(dst, src, length);
}
