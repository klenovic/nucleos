#ifndef EXT2_PROTO_H
#define EXT2_PROTO_H

/* Function prototypes. */

/* Structs used in prototypes must be declared as such first. */
struct buf;
struct filp;
struct ext2_inode;
struct ext2_super_block;

/* balloc.c */
void discard_preallocated_blocks(struct ext2_inode *rip);
block_t alloc_block(struct ext2_inode *rip, block_t goal);
void free_block(struct ext2_super_block *sp, u32 bit);

/* cache.c */
void buf_pool(int bufs);
void flushall(dev_t dev);
struct buf *get_block(dev_t dev, block_t block,int only_search);
void invalidate(dev_t device);
void put_block(struct buf *bp, int block_type);
void set_blocksize(unsigned int blocksize);
void rw_scattered(dev_t dev, struct buf **bufq, int bufqsize, int rw_flag);

/* device.c */
int block_dev_io(int op, dev_t dev, endpoint_t proc_e, void *buf, u64 pos,
		 size_t bytes);
int dev_open(endpoint_t driver_e, dev_t dev, endpoint_t proc_e, int flags);
void dev_close(endpoint_t driver_e, dev_t dev);
int fs_new_driver(void);

/* ialloc.c */
struct ext2_inode *alloc_inode(struct ext2_inode *parent, mode_t bits);
void free_inode(struct ext2_inode *rip);

/* inode.c */
void dup_inode(struct ext2_inode *ip);
struct ext2_inode *find_inode(dev_t dev, ino_t numb);
int fs_putnode(void);
void init_inode_cache(void);
struct ext2_inode *get_inode(dev_t dev, ino_t numb);
void put_inode(struct ext2_inode *rip);
void update_times(struct ext2_inode *rip);
void rw_inode(struct ext2_inode *rip, int rw_flag);

/* link.c */
int fs_ftrunc(void);
int fs_link(void);
int fs_rdlink(void);
int fs_rename(void);
int fs_unlink(void);
int truncate_inode(struct ext2_inode *rip, off_t len);

/* misc.c */
int fs_flush(void);
int fs_sync(void);

/* mount.c */
int fs_mountpoint(void);
int fs_readsuper(void);
int fs_unmount(void);

/* open.c */
int fs_create(void);
int fs_inhibread(void);
int fs_mkdir(void);
int fs_mknod(void);
int fs_slink(void);

/* path.c */
int fs_lookup(void);
struct ext2_inode *advance(struct ext2_inode *dirp, char string[NAME_MAX + 1], int chk_perm);
int search_dir(struct ext2_inode *ldir_ptr, char string [NAME_MAX + 1], ino_t *numb, int flag,
	       int check_permissions, int ftype);

/* protect.c */
int fs_chmod(void);
int fs_chown(void);
int fs_getdents(void);
int forbidden(struct ext2_inode *rip, mode_t access_desired);
int read_only(struct ext2_inode *ip);

/* read.c */
int fs_breadwrite(void);
int fs_readwrite(void);
void read_ahead(void);
block_t read_map(struct ext2_inode *rip, off_t pos);
block_t rd_indir(struct buf *bp, int index);

/* stadir.c */
int fs_fstatfs(void);
int fs_stat(void);
int fs_statvfs(void);

/* super.c */
unsigned int get_block_size(dev_t dev);
struct ext2_super_block *get_super(dev_t dev);
int read_super(struct ext2_super_block *sp);
void write_super(struct ext2_super_block *sp);
struct group_desc* get_group_desc(unsigned int bnum);

/* time.c */
int fs_utime(void);

/* utility.c */
time_t clock_time(void);
unsigned conv2(int norm, int w);
long conv4(int norm, long x);
void mfs_nul_f(char *file, int line, char *str, unsigned int len,
	       unsigned int maxlen);
int no_sys(void);
void sanitycheck(char *file, int line);
#define SANITYCHECK sanitycheck(__FILE__, __LINE__)
int ansi_strcmp(register const char* ansi_s, register const char *s2,
		register size_t ansi_s_length);
u32 setbit(bitchunk_t *bitmap, u32 max_bits, unsigned int word);
u32 setbyte(bitchunk_t *bitmap, u32 max_bits,
			    unsigned int word);
int unsetbit(bitchunk_t *bitmap, u32 bit);

/* write.c */
struct buf *new_block(struct ext2_inode *rip, off_t position);
void zero_block(struct buf *bp);
int write_map(struct ext2_inode *, off_t, block_t, int);

#endif /* EXT2_PROTO_H */
