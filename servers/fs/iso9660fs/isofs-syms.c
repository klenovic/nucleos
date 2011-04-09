/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/types.h>
#include "glo.h"

/* Definition of isofs symbols. */
char *vfs_slink_storage;
dev_t fs_dev;				/* the device that is handled by this FS proc */
char fs_dev_label[FS_DEV_LABEL_LEN];	/* Name of the device driver that is handled */

int err_code;			/* temporary storage for error number */
short path_processed;		/* number of characters processed */
char user_path[USER_PATH_LEN];	/* pathname to be processed */
int symloop;
int rdwt_err;			/* status of last disk i/o request */
