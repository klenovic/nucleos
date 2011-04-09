/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __SERVERS_VFS_LOCK_H
#define __SERVERS_VFS_LOCK_H

/* This is the file locking table.  Like the filp table, it points to the
 * inode table, however, in this case to achieve advisory locking.
 */
struct file_lock {
  short lock_type;		/* F_RDLOCK or F_WRLOCK; 0 means unused slot */
  pid_t lock_pid;		/* pid of the process holding the lock */
  struct vnode *lock_vnode;
  off_t lock_first;		/* offset of first byte locked */
  off_t lock_last;		/* offset of last byte locked */
};

#define NIL_LOCK (struct file_lock *) 0

extern struct file_lock file_lock[NR_LOCKS];

#endif /* __SERVERS_VFS_LOCK_H */
