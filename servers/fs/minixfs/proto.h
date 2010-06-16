/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* Function prototypes. */
#include <servers/mfs/type.h>

/* Structs used in prototypes must be declared as such first. */
struct buf;
struct filp;		
struct inode;
struct minix3_super_block;


/* cache.c */
zone_t alloc_zone(dev_t dev, zone_t z);
void buf_pool(void);
void flushall(dev_t dev);
void free_zone(dev_t dev, zone_t numb);
struct buf *get_block(dev_t dev, block_t block,int only_search);
void invalidate(dev_t device);
void put_block(struct buf *bp, int block_type);
void set_blocksize(int blocksize);
void rw_scattered(dev_t dev,
 struct buf **bufq, int bufqsize, int rw_flag);

/* device.c */
int block_dev_io(int op, dev_t dev, int proc, void *buf,
 u64_t pos, int bytes, int flags);
int dev_open(endpoint_t driver_e, dev_t dev, int proc,
							int flags);
void dev_close(endpoint_t driver_e, dev_t dev);
int fs_clone_opcl(void);
int fs_new_driver(void);

/* inode.c */
struct inode *alloc_inode(dev_t dev, mode_t bits);
void dup_inode(struct inode *ip);
struct inode *find_inode(dev_t dev, int numb);
void free_inode(dev_t dev, ino_t numb);
int fs_getnode(void);
int fs_putnode(void);
void init_inode_cache(void);
struct inode *get_inode(dev_t dev, int numb);
void put_inode(struct inode *rip);
void update_times(struct inode *rip);
void rw_inode(struct inode *rip, int rw_flag);
void wipe_inode(struct inode *rip);

/* link.c */
int freesp_inode(struct inode *rip, off_t st, off_t end);
int fs_ftrunc(void);
int fs_link(void);
int fs_rdlink(void);
int fs_rename(void);
int fs_unlink(void);
int truncate_inode(struct inode *rip, off_t len);

/* main.c */
void reply(int who, kipc_msg_t *m_out);

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
int fs_newnode(void);
int fs_slink(void);

/* path.c */
int fs_lookup(void);
struct inode *advance(struct inode *dirp,
				char string[NAME_MAX], int chk_perm);
int search_dir(struct inode *ldir_ptr,
			char string [NAME_MAX], ino_t *numb, int flag,
			int check_permissions);

/* protect.c */
int fs_chmod(void);
int fs_chown(void);
int fs_getdents(void);
int forbidden(struct inode *rip, mode_t access_desired);
int read_only(struct inode *ip);

/* read.c */
int fs_breadwrite(void);
int fs_readwrite(void);
struct buf *rahead(struct inode *rip, block_t baseblock,
 u64_t position, unsigned bytes_ahead);
void read_ahead(void);
block_t read_map(struct inode *rip, off_t pos);
int read_write(int rw_flag);
zone_t rd_indir(struct buf *bp, int index);

/* stadir.c */
int fs_fstatfs(void);
int fs_stat(void);

/* super.c */
u32 alloc_bit(struct minix3_super_block *sp, int map, u32 origin);
void free_bit(struct minix3_super_block *sp, int map,
 u32 bit_returned);
int get_block_size(dev_t dev);
struct minix3_super_block *get_super(dev_t dev);
int mounted(struct inode *rip);
int read_super(struct minix3_super_block *sp);

/* time.c */
int fs_utime(void);

/* utility.c */
time_t clock_time(void);
unsigned conv2(int norm, int w);
long conv4(int norm, long x);
int fetch_name(char *path, int len, int flag);
void mfs_nul_f(char *file, int line, char *str, int len, 
				int maxlen);
int mfs_min_f(char *file, int line, int len1, int len2);
int no_sys(void);
int isokendpt_f(char *f, int l, int e, int *p, int ft);
void sanitycheck(char *file, int line);
#define SANITYCHECK sanitycheck(__FILE__, __LINE__)

/* write.c */
void clear_zone(struct inode *rip, off_t pos, int flag);
struct buf *new_block(struct inode *rip, off_t position);
void zero_block(struct buf *bp);
int write_map(struct inode *, off_t, zone_t, int);
