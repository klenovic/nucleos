/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* utime(2) for POSIX		Authors: Terrence W. Holm & Edwin L. Froese */

#include <nucleos/lib.h>
#include <nucleos/string.h>
#include <nucleos/utime.h>

int utime(const char *name, const struct utimbuf *timp)
{
  message m;

  if (timp == NULL) {
	m.m2_i1 = 0;		/* name size 0 means NULL `timp' */
	m.m2_i2 = strlen(name) + 1;	/* actual size here */
  } else {
	m.m2_l1 = timp->actime;
	m.m2_l2 = timp->modtime;
	m.m2_i1 = strlen(name) + 1;
  }
  m.m2_p1 = (char *) name;
  return(ksyscall(FS_PROC_NR, __NR_utime, &m));
}
