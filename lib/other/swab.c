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
/*  swab(3)
 *
 *  Author: Terrence W. Holm          Sep. 1988
 */
_PROTOTYPE( void swab, (char *from, char *to, int count));

void swab(from, to, count)
char *from;
char *to;
int count;
{
  register char temp;

  count >>= 1;

  while (--count >= 0) {
	temp = *from++;
	*to++ = *from++;
	*to++ = temp;
  }
}
