#include <nucleos/vfsif.h>
#include <nucleos/kipc.h>
#include <nucleos/queue.h>
#include <nucleos/dmap.h>
#include "const.h"
#include "type.h"
#include "inode.h"
#include "drivers.h"

/* The following variables are used for returning results to the caller. */
int err_code;        /* temporary storage for error number */
int rdwt_err;        /* status of last disk i/o request */

int cch[NR_INODES];

kipc_msg_t fs_m_in;
kipc_msg_t fs_m_out;
vfs_ucred_t credentials;

uid_t caller_uid;
gid_t caller_gid;

int req_nr;

endpoint_t SELF_E;

char user_path[PATH_MAX+1];  /* pathname to be processed */

dev_t fs_dev;              /* The device that is handled by this FS proc
                                   */
char fs_dev_label[16];    /* Name of the device driver that is handled
                                  * by this FS proc.
                                 */
int unmountdone;
int exitsignaled;

/* our block size. */
unsigned int fs_block_size;

/* Buffer cache. */
struct buf *buf;
struct buf **buf_hash;   /* the buffer hash table */
unsigned int nr_bufs;
int may_use_vmcache;
/* Little hack for syncing group descriptors. */
int group_descriptors_dirty;

struct opt opt;		/* global options */

/* On ext2 metadata is stored in little endian format, so we shoud take
 * care about byte swapping, when have BE CPU. */
int le_CPU;	/* little/big endian, if TRUE do not swap bytes */

/* list of unused/free inodes */
struct unused_inodes_t unused_inodes;

/* inode hashtable */
struct inodelist hash_inodes[INODE_HASH_SIZE];

unsigned int inode_cache_hit;
unsigned int inode_cache_miss;
struct inode inode[NR_INODES];

struct super_block *superblock;
struct super_block *ondisk_superblock;

struct driver_endpoints driver_endpoints[NR_DEVICES];

struct buf *front;		/* points to least recently used free block */
struct buf *rear;		/* points to most recently used free block */
unsigned int bufs_in_use;	/* # bufs currently in use (not on free list)*/
