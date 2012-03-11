/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __SERVERS_VFS_REQUEST_H
#define __SERVERS_VFS_REQUEST_H

/* Low level request messages are built and sent by wrapper functions.
 * This file contains the request and response structures for accessing
 * those wrappers functions.
 */

#include <nucleos/types.h>

/* Structure for response that contains inode details */
typedef struct node_details {
	endpoint_t fs_e;
	ino_t inode_nr;
	mode_t fmode;
	off_t fsize;
	uid_t uid;
	gid_t gid;

	/* For faster access */
	unsigned short inode_index;

	/* For char/block special files */
	dev_t dev;
} node_details_t;

/* Structure for a lookup response */
typedef struct lookup_res {
	endpoint_t fs_e;
	ino_t inode_nr;
	mode_t fmode;
	off_t fsize;
	uid_t uid;
	gid_t gid;
	/* For char/block special files */
	dev_t dev;
	
	/* Fields used for handling mount point and symbolic links */
	int char_processed;
	unsigned char symloop;
} lookup_res_t;


/* Structure for REQ_ request */
#endif /* __SERVERS_VFS_REQUEST_H */
