/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* ttyname.c						POSIX 4.7.2
 *	char *ttyname(int fildes);
 *
 *	Determines name of a terminal device.
 */

#include <lib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <nucleos/stddef.h>
#include <string.h>
#include <unistd.h>

static char base[] = "/dev";
static char path[sizeof(base) + 1 + NAME_MAX];	/* extra 1 for '/' */

char *ttyname(fildes)
int fildes;
{
  DIR *devices;
  struct dirent *entry;
  struct stat tty_stat;
  struct stat dev_stat;

  /* Simple first test: file descriptor must be a character device */
  if (fstat(fildes, &tty_stat) < 0 || !S_ISCHR(tty_stat.st_mode))
	return (char *) NULL;

  /* Open device directory for reading  */
  if ((devices = opendir(base)) == (DIR *) NULL)
	return (char *) NULL;

  /* Scan the entries for one that matches perfectly */
  while ((entry = readdir(devices)) != (struct dirent *) NULL) {
	if (tty_stat.st_ino != entry->d_ino)
		continue;
	strcpy(path, base);
	strcat(path, "/");
	strcat(path, entry->d_name);
	if (stat(path, &dev_stat) < 0 || !S_ISCHR(dev_stat.st_mode))
		continue;
	if (tty_stat.st_ino == dev_stat.st_ino &&
	    tty_stat.st_dev == dev_stat.st_dev &&
	    tty_stat.st_rdev == dev_stat.st_rdev) {
		closedir(devices);
		return path;
	}
  }

  closedir(devices);
  return (char *) NULL;
}
