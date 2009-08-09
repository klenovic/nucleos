/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* getgroups.c						POSIX 4.2.3
 *	int getgroups(gidsetsize, grouplist);
 *
 *	This call relates to suplementary group ids, which are not
 *	supported in MINIX.
 */

#include <lib.h>
#define getgroups _getgroups
#include <unistd.h>
#include <time.h>

int getgroups(gidsetsize, grouplist)
int gidsetsize;
gid_t grouplist[];
{
  return(0);
}
