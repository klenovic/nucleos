/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/lib.h>
#include <nucleos/unistd.h>

extern void *__curbrk;

char *sbrk(int incr)
{
  char *newsize, *oldsize;

  oldsize = __curbrk;
  newsize = __curbrk + incr;
  if ((incr > 0 && newsize < oldsize) || (incr < 0 && newsize > oldsize))
	return( (char *) -1);
  if (brk(newsize) == 0)
	return(oldsize);
  else
	return( (char *) -1);
}
