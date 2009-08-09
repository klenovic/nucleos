/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*  getlogin(3)
 *
 *  Author: Terrence W. Holm          Aug. 1988
 */

#include <lib.h>
#include <pwd.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#ifndef  L_cuserid
#define  L_cuserid   9
#endif

char *getlogin()
{
  static char userid[L_cuserid];
  struct passwd *pw_entry;

  pw_entry = getpwuid(getuid());

  if (pw_entry == (struct passwd *)NULL) return((char *)NULL);

  strcpy(userid, pw_entry->pw_name);

  return(userid);
}
