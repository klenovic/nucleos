/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*  cuserid(3)
 *
 *  Author: Terrence W. Holm          Sept. 1987
 */

#include <nucleos/lib.h>
#include <pwd.h>
#include <nucleos/string.h>
#include <nucleos/unistd.h>
#include <stdio.h>

#ifndef  L_cuserid
#define  L_cuserid   9
#endif

char *cuserid(user_name)
char *user_name;
{
  static char userid[L_cuserid];
  struct passwd *pw_entry;

  if (user_name == (char *)NULL) user_name = userid;

  pw_entry = getpwuid(geteuid());

  if (pw_entry == (struct passwd *)NULL) {
	*user_name = '\0';
	return((char *)NULL);
  }
  strcpy(user_name, pw_entry->pw_name);

  return(user_name);
}
