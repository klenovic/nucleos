/*
 *  Copyright (C) 2010  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* mmap implementation in VFS
 *
 * The entry points into this file are
 *   do_vm_mmap:	VM calls VM_VFS_MMAP
 */

#include "fs.h"
#include <nucleos/stat.h>
#include <nucleos/fcntl.h>
#include <nucleos/string.h>
#include <nucleos/unistd.h>
#include <nucleos/com.h>
#include <nucleos/u64.h>
#include "file.h"
#include <servers/vfs/fproc.h>
#include "lock.h"
#include "param.h"
#include <nucleos/dirent.h>
#include <assert.h>

#include <nucleos/vfsif.h>
#include "vnode.h"
#include "vmnt.h"

/*===========================================================================*
 *				do_vm_mmap		     		*
 *===========================================================================*/
int do_vm_mmap()
{
}

