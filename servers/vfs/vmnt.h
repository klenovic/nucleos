/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __SERVERS_VFS_VMNT_H
#define __SERVERS_VFS_VMNT_H

EXTERN struct vmnt {
  int m_fs_e;                   /* FS process' kernel endpoint */
  dev_t m_dev;                  /* device number */
  int m_flags;                  /* mount flags */
  struct vnode *m_mounted_on;   /* the vnode on which the partition is mounted */
  struct vnode *m_root_node;    /* root vnode */
} vmnt[NR_MNTS];

#define NIL_VMNT (struct vmnt *) 0

#endif /* __SERVERS_VFS_VMNT_H */
