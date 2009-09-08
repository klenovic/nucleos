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
/* index - find first occurrence of a character in a string */

#include <nucleos/string.h>

char *index(s, charwanted)	/* found char, or NULL if none */
const char *s;
char charwanted;
{
  return(strchr(s, charwanted));
}
