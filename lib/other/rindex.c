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
/* rindex - find last occurrence of a character in a string  */

#include <string.h>

char *rindex(s, charwanted)	/* found char, or NULL if none */
_CONST char *s;
char charwanted;
{
  return(strrchr(s, charwanted));
}
