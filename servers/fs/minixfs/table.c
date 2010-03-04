/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

/* This file contains the table used to map system call numbers onto the
 * routines that perform them.
 */
#include "fs.h"
#include <nucleos/unistd.h>
#include <nucleos/com.h>
#include <servers/mfs/inode.h>
#include <servers/mfs/buf.h>
#include <servers/mfs/super.h>
#include "drivers.h"

int (*fs_call_vec[])(void) = {
        no_sys,             /* 0   not used */
        no_sys,             /* 1   */	    /* Was: fs_getnode */
        fs_putnode,         /* 2   */
        fs_slink,           /* 3   */
        fs_ftrunc,          /* 4   */
        fs_chown,           /* 5   */
	fs_chmod,           /* 6   */
        fs_inhibread,       /* 7   */
        fs_stat,            /* 8   */
        fs_utime,           /* 9   */
        fs_fstatfs,         /* 10  */
        fs_breadwrite,      /* 11  */
        fs_breadwrite,      /* 12  */
        fs_unlink,	    /* 13  */
        fs_unlink,          /* 14  */
        fs_unmount,         /* 15  */
	fs_sync,            /* 16  */
        fs_new_driver,      /* 17  */
        fs_flush,	    /* 18  */
        fs_readwrite,	    /* 19  */
        fs_readwrite,	    /* 20  */
        fs_mknod,           /* 21  */
        fs_mkdir,           /* 22  */
        fs_create,          /* 23  */
        fs_link,            /* 24  */
        fs_rename,          /* 25  */
        fs_lookup,          /* 26  */
        fs_mountpoint,      /* 27  */
        fs_readsuper,	    /* 28  */
        fs_newnode,	    /* 29  */
        fs_rdlink,	    /* 30  */
        fs_getdents,	    /* 31  */
};
