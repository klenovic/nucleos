/* Function prototypes. */
#include <nucleos/types.h>
#include "type.h"

/* Structs used in prototypes must be declared as such first. */
struct buf;
struct filp;
struct pipe_inode;

/* buffer.c */
struct buf *get_block(dev_t dev, ino_t inum);
struct buf *new_block(dev_t dev, ino_t inum);
void put_block(dev_t dev, ino_t inum);

/* cache.c */
void buf_pool(void);

/* inode.c */
struct pipe_inode *alloc_inode(dev_t dev, mode_t mode);
void dup_inode(struct pipe_inode *ip);
struct pipe_inode *find_inode(int numb);
void free_inode(struct pipe_inode *rip);
int fs_putnode(void);
void init_inode_cache(void);
struct pipe_inode *get_inode(dev_t dev, int numb);
void put_inode(struct pipe_inode *rip);
void update_times(struct pipe_inode *rip);
void wipe_inode(struct pipe_inode *rip);

/* link.c */
int fs_ftrunc(void);
int truncate_inode(struct pipe_inode *rip, off_t newsize);


/* main.c */
void reply(int who, kipc_msg_t *m_out);

/* misc.c */
int fs_sync(void);

/* open.c */
int fs_newnode(void);

/* read.c */
int fs_readwrite(void);
block_t read_map(struct pipe_inode *rip, off_t pos);
int read_write(int rw_flag);

/* utility.c */
time_t clock_time(void);
int no_sys(void);

/* stadir.c */
int fs_stat(void);

/* super.c */
u32 alloc_bit(void);
void free_bit(u32 bit_returned);
