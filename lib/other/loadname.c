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
#include <string.h>

PUBLIC void _loadname(name, msgptr)
_CONST char *name;
message *msgptr;
{
/* This function is used to load a string into a type m3 message. If the
 * string fits in the message, it is copied there.  If not, a pointer to
 * it is passed.
 */

  register size_t k;

  k = strlen(name) + 1;
  msgptr->m3_i1 = k;
  msgptr->m3_p1 = (char *) name;
  if (k <= sizeof msgptr->m3_ca1) strcpy(msgptr->m3_ca1, name);
}
