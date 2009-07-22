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
/*  lsearch(3)  and  lfind(3)
 *
 *  Author: Terrence W. Holm          Sep. 1988
 */

#include <nucleos/stddef.h>

_PROTOTYPE( char *lsearch, (char *key, char *base,
			    unsigned *count, unsigned width,
			    int (*keycmp)(const void *, const void *)));
_PROTOTYPE( char *lfind, (char *key, char *base,
			    unsigned *count, unsigned width,
			    int (*keycmp)(const void *, const void *)));

char *lsearch(key, base, count, width, keycmp)
char *key;
char *base;
unsigned *count;
unsigned width;
_PROTOTYPE( int (*keycmp), (const void *, const void *));
{
  char *entry;
  char *last = base + *count * width;

  for (entry = base; entry < last; entry += width)
	if (keycmp(key, entry) == 0) return(entry);

  bcopy(key, last, width);
  *count += 1;
  return(last);
}


char *lfind(key, base, count, width, keycmp)
char *key;
char *base;
unsigned *count;
unsigned width;
_PROTOTYPE( int (*keycmp), (const void *, const void *));
{
  char *entry;
  char *last = base + *count * width;

  for (entry = base; entry < last; entry += width)
	if (keycmp(key, entry) == 0) return(entry);

  return((char *)NULL);
}
