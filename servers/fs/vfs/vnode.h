/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __SERVERS_VFS_VNODE_H
#define __SERVERS_VFS_VNODE_H

struct vnode {
	endpoint_t v_fs_e;	/* FS process' endpoint number */
	endpoint_t v_mapfs_e;	/* mapped FS process' endpoint number */
	ino_t v_inode_nr;	/* inode number on its (minor) device */
	ino_t v_mapinode_nr;	/* mapped inode number of mapped FS. */
	mode_t v_mode;		/* file type, protection, etc. */
	uid_t v_uid;		/* uid of inode. */
	gid_t v_gid;		/* gid of inode. */
	off_t v_size;		/* current file size in bytes */
	int v_ref_count;	/* # times vnode used; 0 means slot is free */
	int v_fs_count;		/* # reference at the underlying FS */
	int v_mapfs_count;		/* # reference at the underlying mapped FS */
#if 0
	int v_ref_check;	/* for consistency checks */
#endif
	char v_pipe;		/* set to I_PIPE if pipe */
	off_t v_pipe_rd_pos;
	off_t v_pipe_wr_pos;
	endpoint_t v_bfs_e;	/* endpoint number for the FS proces in case
				   of a block special file */
	dev_t v_dev;		/* device number on which the corresponding
				   inode resides */
	dev_t v_sdev;		/* device number for special files */
	struct vmnt *v_vmnt;	/* vmnt object of the partition */
};

extern struct vnode vnode[];

#define NIL_VNODE	(struct vnode *) 0	/* indicates absence of vnode slot */

/* Field values. */
#define NO_PIPE		0	/* i_pipe is NO_PIPE if inode is not a pipe */
#define I_PIPE		1	/* i_pipe is I_PIPE if inode is a pipe */

#endif /* __SERVERS_VFS_VNODE_H */
