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
#include <nucleos/types.h>
#include <unistd.h>

void std_err(char *s);

void std_err(s)
char *s;
{
  register char *p = s;

  while (*p != 0) p++;
  write(2, s, (int) (p - s));
}
