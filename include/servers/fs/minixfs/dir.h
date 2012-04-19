/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The <dir.h> header gives the layout of a directory. */

#ifndef _SERVERS_FS_MINIXFS_DIR_H
#define _SERVERS_FS_MINIXFS_DIR_H

#include <nucleos/types.h>

#define DIRBLKSIZ	512	/* size of directory block */

#ifndef MINIXFS_DIRSIZ
#define MINIXFS_DIRSIZ		60
#endif

struct direct {
	ino_t d_ino;
	char d_name[MINIXFS_DIRSIZ];
};

#endif /* _SERVERS_FS_MINIXFS_DIR_H */
