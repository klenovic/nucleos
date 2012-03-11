/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef _NUCLEOS_VFSIF_H
#define _NUCLEOS_VFSIF_H

#include <nucleos/types.h>
#include <nucleos/limits.h>

/* VFS/FS request fields */
#define REQ_ACTIME		m_data2
#define REQ_COUNT		m_data2
#define REQ_DEV			m_data5
#define REQ_DEV2		m_data1
#define REQ_DIR_INO   		m_data3
#define REQ_DRIVER_E		m_data2
#define REQ_FLAGS		m_data8
#define REQ_GID			m_data6
#define REQ_GRANT		m_data2
#define REQ_GRANT2		m_data1
#define REQ_GRANT3		m_data3
#define REQ_INODE_NR		m_data1
#define REQ_MEM_SIZE		m_data5
#define REQ_MODE		m_data8
#define REQ_MODTIME		m_data3
#define REQ_NBYTES		m_data5
#define REQ_PATH_LEN		m_data7
#define REQ_PATH_SIZE		m_data5
#define REQ_REN_GRANT_NEW	m_data1
#define REQ_REN_GRANT_OLD	m_data2
#define REQ_REN_LEN_NEW		m_data7
#define REQ_REN_LEN_OLD		m_data6
#define REQ_REN_NEW_DIR		m_data4
#define REQ_REN_OLD_DIR		m_data3
#define REQ_ROOT_INO		m_data4
#define REQ_SEEK_POS_HI		m_data3
#define REQ_SEEK_POS_LO		m_data4
#define REQ_TRC_END_HI		m_data4
#define REQ_TRC_END_LO		m_data5
#define REQ_TRC_START_HI	m_data2
#define REQ_TRC_START_LO	m_data3
#define REQ_UCRED_SIZE		m_data9
#define REQ_UID			m_data9

/* VFS/FS reply fields */
#define RES_DEV			m_data4
#define RES_GID			m_data6
#define RES_INODE_NR		m_data1
#define RES_FILE_SIZE_HI	m_data2
#define RES_FILE_SIZE_LO	m_data3
#define RES_MODE		m_data7
#define RES_NBYTES		m_data5
#define RES_OFFSET		m_data7
#define RES_SEEK_POS_HI		m_data3
#define RES_SEEK_POS_LO		m_data4
#define RES_SYMLOOP		m_data8
#define RES_UID			m_data9

/* VFS/FS flags */
#define REQ_RDONLY		001
#define REQ_ISROOT		002
#define PATH_NOFLAGS		000
#define PATH_RET_SYMLINK	010	/* Return a symlink object (i.e.
					 * do not continue with the contents
					 * of the symlink if it is the last
					 * component in a path). */
#define PATH_GET_UCRED		020	/* Request provides a grant ID in m_data1
					 * and struct ucred size in m_data9 (as
					 * opposed to a REQ_UID). */

/* VFS/FS error messages */
#define EENTERMOUNT	301
#define ELEAVEMOUNT	302
#define ESYMLINK	303

/* VFS/FS types */

/* User credential structure */
typedef struct {
	uid_t vu_uid;
	gid_t vu_gid;
	int vu_ngroups;
	gid_t vu_sgroups[NGROUPS_MAX];
} vfs_ucred_t;

/* Request numbers */
#define REQ_GETNODE	(VFS_BASE + 1)	/* Should be removed */
#define REQ_PUTNODE	(VFS_BASE + 2)
#define REQ_SLINK	(VFS_BASE + 3)
#define REQ_FTRUNC	(VFS_BASE + 4)
#define REQ_CHOWN	(VFS_BASE + 5)
#define REQ_CHMOD	(VFS_BASE + 6)
#define REQ_INHIBREAD	(VFS_BASE + 7)
#define REQ_STAT	(VFS_BASE + 8)
#define REQ_UTIME	(VFS_BASE + 9)
#define REQ_FSTATFS	(VFS_BASE + 10)
#define REQ_BREAD	(VFS_BASE + 11)
#define REQ_BWRITE	(VFS_BASE + 12)
#define REQ_UNLINK	(VFS_BASE + 13)
#define REQ_RMDIR	(VFS_BASE + 14)
#define REQ_UNMOUNT	(VFS_BASE + 15)
#define REQ_SYNC	(VFS_BASE + 16)
#define REQ_NEW_DRIVER	(VFS_BASE + 17)
#define REQ_FLUSH	(VFS_BASE + 18)
#define REQ_READ	(VFS_BASE + 19)
#define REQ_WRITE	(VFS_BASE + 20)
#define REQ_MKNOD	(VFS_BASE + 21)
#define REQ_MKDIR	(VFS_BASE + 22)
#define REQ_CREATE	(VFS_BASE + 23)
#define REQ_LINK	(VFS_BASE + 24)
#define REQ_RENAME	(VFS_BASE + 25)
#define REQ_LOOKUP	(VFS_BASE + 26)
#define REQ_MOUNTPOINT  (VFS_BASE + 27)
#define REQ_READSUPER	(VFS_BASE + 28)
#define REQ_NEWNODE	(VFS_BASE + 29)
#define REQ_RDLINK	(VFS_BASE + 30)
#define REQ_GETDENTS	(VFS_BASE + 31)
#define REQ_STATVFS	(VFS_BASE + 32)
#define NREQS		33

#endif /* _NUCLEOS_VFSIF_H */
