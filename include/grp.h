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

#ifndef _TYPES_H
#include <sys/types.h>
#endif

struct	group { 
  char *gr_name;		/* the name of the group */
  char *gr_passwd;		/* the group passwd */
  gid_t gr_gid;			/* the numerical group ID */
  char **gr_mem;		/* a vector of pointers to the members */
};

/* Function Prototypes. */
_PROTOTYPE( struct group *getgrgid, (_mnx_Gid_t _gid)  			);
_PROTOTYPE( struct group *getgrnam, (const char *_name)			);

#ifdef _MINIX
_PROTOTYPE( void endgrent, (void)					);
_PROTOTYPE( struct group *getgrent, (void)				);
_PROTOTYPE( int setgrent, (void)					);
_PROTOTYPE( void setgrfile, (const char *_file)				);
#endif

#endif /* _GRP_H */
