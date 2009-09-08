/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/lib.h>
#include <nucleos/string.h>
#include <nucleos/mount.h>
#include <stdlib.h>
#include <stdio.h>
#include <nucleos/stat.h>
#include <nucleos/syslib.h>
#include <servers/rs/rs.h>
#include <nucleos/paths.h>

#define FSPATH "/sbin/"
#define FSDEFAULT "mfs"

static int rs_down(char *label)
{
	char cmd[200];
	message m;
	if(strlen(_PATH_SERVICE)+strlen(label)+50 >= sizeof(cmd))
		return -1;
	sprintf(cmd, _PATH_SERVICE " down '%s'", label);
	return system(cmd);
}

static char *makelabel(const char *special)
{
  static char label[40];
  const char *dev;

  /* Make label name. */
  dev = strrchr(special, '/');
  if(dev) dev++;
  else dev = special;
  if(strchr(dev, '\'') != NULL) {
  	errno = EINVAL;
  	return NULL;
  }
  if(strlen(dev)+4 >= sizeof(label)) {
  	errno = E2BIG;
	return NULL;
  }
  sprintf(label, "fs_%s", dev);
  return label;
}

int mount(special, name, mountflags, type, args)
char *name, *special, *type, *args;
int mountflags;
{
  int r;
  message m;
  struct rs_start rs_start;
  struct stat statbuf;
  char *label;
  char path[60];
  char cmd[200];
  FILE *pipe;
  int ep;
  int reuse;

  /* Default values. */
  if (type == NULL) type = FSDEFAULT;
  if (args == NULL) args = "";
  reuse = 0;

  /* Check mount flags */
  if(mountflags & MS_REUSE) {
    reuse = 1;
    mountflags &= ~MS_REUSE; /* Temporary: turn off to not confuse VFS */
  }

  /* Make FS_PROC_NR process label for RS from special name. */
  if(!(label=makelabel(special))) {
	return -1;
  }

  /* See if the given type is even remotely valid. */
  if(strlen(FSPATH)+strlen(type) >= sizeof(path)) {
	errno = E2BIG;
	return -1;
  }
  strcpy(path, FSPATH);
  strcat(path, type);
  
  if(stat(path, &statbuf) != 0) {
  	errno = EINVAL;
  	return -1;
  }

  /* Sanity check on user input. */
  if(strchr(args, '\'')) {
  	errno = EINVAL;
	return -1;
  }

  if(strlen(_PATH_SERVICE)+strlen(path)+strlen(label)+
     strlen(_PATH_DRIVERS_CONF)+strlen(args)+50 >= sizeof(cmd)) {
	errno = E2BIG;
	return -1;
  }



  sprintf(cmd, _PATH_SERVICE " %sup %s -label '%s' -config " _PATH_DRIVERS_CONF
	" -args '%s%s' -printep yes",
	  reuse ? "-r ": "", path, label, args[0] ? "-o " : "", args);

  if(!(pipe = popen(cmd, "r"))) {
	fprintf(stderr, "mount: couldn't run %s\n", cmd);
	return -1;
  }
  
  if(fscanf(pipe, "%d", &ep) != 1 || ep <= 0) {
	fprintf(stderr, "mount: couldn't parse endpoint from %s\n", cmd);
	errno = EINVAL;
	pclose(pipe);
	return -1;
  }
  pclose(pipe);
  
  /* Now perform mount(). */
  m.m1_i1 = strlen(special) + 1;
  m.m1_i2 = strlen(name) + 1;
  m.m1_i3 = mountflags;
  m.m1_p1 = special;
  m.m1_p2 = name;
  m.m1_p3 = (char*) ep;
  r = _syscall(FS_PROC_NR, __NR_mount, &m);

  if(r != 0) {
	/* If mount() failed, tell RS to shutdown MFS process.
	 * No error check - won't do anything with this error anyway.
	 */
	rs_down(label);
  }

  return r;
}

int umount(name)
const char *name;
{
  message m;
  char *label;
  int r;

  /* Make MFS process label for RS from special name. */
  if(!(label=makelabel(name))) {
	return -1;
  }

  _loadname(name, &m);
  r = _syscall(FS_PROC_NR, __NR_umount, &m);

  if(r == 0) {
	rs_down(label);
  }

  return r;
}
