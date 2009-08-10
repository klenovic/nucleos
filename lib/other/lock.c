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
#include <errno.h>
#include <nucleos/types.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
  False, True
} BOOLEAN;

#define LOCKDIR "/tmp/"		/* or /usr/tmp/ as the case may be */
#define MAXTRIES 3
#define NAPTIME (unsigned int)5

static char *lockpath(char *name);
void syserr(char *errstring);
BOOLEAN lock(char *name);
void unlock(char *name);

void syserr(char *errstring)
{
	fprintf(stderr,"couldn't %s\n", errstring);
	exit(1);
}

BOOLEAN lock(char *name)		/* acquire lock */
{
  char *path;
  int fd, tries;

  path = lockpath(name);
  tries = 0;
  while ((fd = creat(path, 0)) == -1 && errno == EACCES) {
	if (++tries >= MAXTRIES) return(False);
	sleep(NAPTIME);
  }
  if (fd == -1 || close(fd) == -1) syserr("lock");
  return(True);
}

void unlock(char *name)		/* free lock */
{
  if (unlink(lockpath(name)) == -1) syserr("unlock");
}

static char *lockpath(char *name)	/* generate lock file path */
{
  static char path[20];

  strcpy(path, LOCKDIR);
  return(strcat(path, name));
}
