/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The <grp.h> header is used for the getgrid() and getgrnam() calls. */

#ifndef _GRP_H
#define _GRP_H

#include <nucleos/types.h>

struct	group { 
  char *gr_name;		/* the name of the group */
  char *gr_passwd;		/* the group passwd */
  gid_t gr_gid;			/* the numerical group ID */
  char **gr_mem;		/* a vector of pointers to the members */
};

/* Function Prototypes. */
struct group *getgrgid(_mnx_Gid_t _gid);
struct group *getgrnam(const char *_name);

#ifdef _MINIX
void endgrent(void);
struct group *getgrent(void);
int setgrent(void);
void setgrfile(const char *_file);
#endif

#endif /* _GRP_H */
