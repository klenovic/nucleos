/* EXTERN should be extern except for the table file */

#ifndef EXT2_GLO_H
#define EXT2_GLO_H

#include <nucleos/vfsif.h>
#include <servers/fs/ext2/const.h>

/* The following variables are used for returning results to the caller. */
extern int err_code;        /* temporary storage for error number */
extern int rdwt_err;        /* status of last disk i/o request */

extern int cch[NR_INODES];

extern char dot1[2];   /* dot1 (&dot1[0]) and dot2 (&dot2[0]) have a special */
extern char dot2[3];   /* meaning to search_dir: no access permission check. */

extern int (*fs_call_vec[])(void); /* fs call table */

extern kipc_msg_t fs_m_in;
extern kipc_msg_t fs_m_out;
extern vfs_ucred_t credentials;

extern uid_t caller_uid;
extern gid_t caller_gid;

extern int req_nr;

extern endpoint_t SELF_E;

extern char user_path[PATH_MAX+1];  /* pathname to be processed */

extern dev_t fs_dev;              /* The device that is handled by this FS proc
                                   */
extern char fs_dev_label[16];    /* Name of the device driver that is handled
                                  * by this FS proc.
                                  */
extern int unmountdone;
extern int exitsignaled;

/* our block size. */
extern unsigned int fs_block_size;

/* Buffer cache. */
extern struct buf *buf;
extern struct buf **buf_hash;   /* the buffer hash table */
extern unsigned int nr_bufs;
extern int may_use_vmcache;
/* Little hack for syncing group descriptors. */
extern int group_descriptors_dirty;

extern struct opt opt;		/* global options */

/* On ext2 metadata is stored in little endian format, so we shoud take
 * care about byte swapping, when have BE CPU. */
extern int le_CPU;	/* little/big endian, if TRUE do not swap bytes */

#endif /* EXT2_GLO_H */
