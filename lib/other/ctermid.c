/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*  ctermid(3)
 *
 *  Author: Terrence Holm          Aug. 1988
 *
 *
 *  Ctermid(3) returns a pointer to a string naming the controlling
 *  terminal. If <name_space> is NULL then local static storage
 *  is used, otherwise <name_space> must point to storage of at
 *  least L_ctermid characters.
 *
 *  Returns a pointer to "/dev/tty".
 */

#include <nucleos/lib.h>
#include <nucleos/string.h>
#include <stdio.h>

char *ctermid(char *name_space);

#ifndef L_ctermid
#define L_ctermid  9
#endif

char *ctermid(name_space)
char *name_space;
{
  static char termid[L_ctermid];

  if (name_space == (char *)NULL) name_space = termid;
  strcpy(name_space, "/dev/tty");
  return(name_space);
}
