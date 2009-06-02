/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* This is the file locking table.  Like the filp table, it points to the
 * inode table, however, in this case to achieve advisory locking.
 */
EXTERN struct file_lock {
  short lock_type;		/* F_RDLOCK or F_WRLOCK; 0 means unused slot */
  pid_t lock_pid;		/* pid of the process holding the lock */
  struct vnode *lock_vnode;
  off_t lock_first;		/* offset of first byte locked */
  off_t lock_last;		/* offset of last byte locked */
} file_lock[NR_LOCKS];
