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
/* bcmp - Berklix equivalent of memcmp  */

#include <string.h>

int bcmp(s1, s2, length)	/* == 0 or != 0 for equality and inequality */ 
_CONST void *s1;
_CONST void *s2;
size_t length;
{
  return(memcmp(s1, s2, length));
}
