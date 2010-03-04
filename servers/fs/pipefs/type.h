#include <nucleos/types.h>

typedef __u32 __kernel_block_t; /* block number */

#if defined (__KERNEL__) || defined(__UKERNEL__)
typedef __kernel_block_t block_t;	/* block number */
#if 0
struct buf {
  /* Data portion of the buffer. */
  union fsdata_u *bp;

  /* Header portion of the buffer. */
  struct buf *b_next;           /* used to link all free bufs in a chain */
  struct buf *b_prev;           /* used to link all free bufs the other way */
  struct buf *b_hash;           /* used to link bufs on hash chains */
  block_t b_blocknr;            /* block number of its (minor) device */
  dev_t b_dev;                  /* major | minor device where block resides */
  char b_dirt;                  /* CLEAN or DIRTY */
  char b_count;                 /* number of users of this buffer */
  int b_bytes;                  /* Number of bytes allocated in bp */
};
#endif
#endif
