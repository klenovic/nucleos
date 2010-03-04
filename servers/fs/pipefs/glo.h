/* extern should be extern except for the table file */
#include <nucleos/vfsif.h>

/* The following variables are used for returning results to the caller. */
extern int err_code;		/* temporary storage for error number */

extern int cch[NR_INODES];

extern int (*fs_call_vec[])(void); /* fs call table */

extern message fs_m_in;
extern message fs_m_out;

extern uid_t caller_uid;
extern gid_t caller_gid;
extern int req_nr;
extern int SELF_E;
extern int exitsignaled;
extern int busy;

/* Inode map. */
extern bitchunk_t inodemap[FS_BITMAP_CHUNKS(NR_INODES)]; 
