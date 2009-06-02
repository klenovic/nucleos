/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The <dir.h> header gives the layout of a directory. */

#ifndef _DIR_H
#define _DIR_H

#include <sys/types.h>

#define	DIRBLKSIZ	512	/* size of directory block */

#ifndef DIRSIZ
#define DIRSIZ	60
#endif

struct direct {
  ino_t d_ino;
  char d_name[DIRSIZ];
};

#endif /* _DIR_H */
