/* extern should be extern except for the table file */
#include <nucleos/vfsif.h>
#include <nucleos/kipc.h>
#include <nucleos/dmap.h>
#include "drivers.h"
#include "inode.h"
#include "const.h"

/* The following variables are used for returning results to the caller. */
int err_code;		/* temporary storage for error number */

int cch[NR_INODES];

kipc_msg_t fs_m_in;
kipc_msg_t fs_m_out;

uid_t caller_uid;
gid_t caller_gid;
int req_nr;
int SELF_E;
int exitsignaled;
int busy;

/* Inode map. */
bitchunk_t inodemap[FS_BITMAP_CHUNKS(NR_INODES)]; 

struct inode inode[NR_INODES];

/* list of unused/free inodes */
struct unused_inodes_t unused_inodes;

/* inode hashtable */
struct inodelist hash_inodes[INODE_HASH_SIZE];

struct buf *front;	/* points to least recently used free block */
struct buf *rear;	/* points to most recently used free block */
int bufs_in_use;	/* # bufs currently in use (not on free list)*/

struct driver_endpoints driver_endpoints[NR_DEVICES];
