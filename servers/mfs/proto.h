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

#include <nucleos/timer.h>

/* Structs used in prototypes must be declared as such first. */
struct buf;
struct filp;		
struct inode;
struct super_block;


int fs_putnode(void);
int fs_getnode(void);
int fs_pipe(void);
int fs_clone_opcl(void);
int fs_new_driver(void);
int fs_ftrunc(void);
int fs_chown(void);
int fs_chmod(void);
int fs_inhibread(void);
int fs_stat(void);
int fs_utime(void);
int fs_fstatfs(void);

int fs_unmount(void);
int fs_trunc(void);
int fs_sync(void);
int fs_stime(void);

int fs_getdents_o(void);
int fs_getdents(void);
int fs_flush(void);

void init_inode_cache(void);

/* cache.c */
zone_t alloc_zone(dev_t dev, zone_t z);
void flushall(dev_t dev);
void free_zone(dev_t dev, zone_t numb);
struct buf *get_block(dev_t dev, block_t block,int only_search);
void invalidate(dev_t device);
void put_block(struct buf *bp, int block_type);
void set_blocksize(int blocksize);
void rw_scattered(dev_t dev, struct buf **bufq, int bufqsize, int rw_flag);
void buf_pool(void);

/* device.c */
int block_dev_io(int op, dev_t dev, int proc, void *buf, u64_t pos, int bytes, int flags);
int dev_open(endpoint_t driver_e, dev_t dev, int proc, int flags);
void dev_close(endpoint_t driver_e, dev_t dev);

/* inode.c */
struct inode *find_inode(dev_t dev, int numb);

struct inode *alloc_inode(dev_t dev, mode_t bits);
void dup_inode(struct inode *ip);
void free_inode(dev_t dev, ino_t numb);
struct inode *get_inode(dev_t dev, int numb);
void put_inode(struct inode *rip);
void update_times(struct inode *rip);
void rw_inode(struct inode *rip, int rw_flag);
void wipe_inode(struct inode *rip);

/* link.c */
int fs_link_o(void);
int fs_link_s(void);
int fs_rdlink_o(void);
int fs_rdlink_so(void);
int fs_rdlink_s(void);
int fs_rename_o(void);
int fs_rename_s(void);
int fs_unlink_o(void);
int fs_unlink_s(void);

int truncate_inode(struct inode *rip, off_t len);
int freesp_inode(struct inode *rip, off_t st, off_t end);

/* lock.c */
int lock_op(struct filp *f, int req);
void lock_revive(void);

/* main.c */
void reply(int who, message *m_out);

/* misc.c */
int do_dup(void);
int do_exit(void);
int do_fcntl(void);
int do_exec(void);
int do_revive(void);
int do_set(void);
int do_sync(void);
int do_fsync(void);
int do_reboot(void);
int do_svrctl(void);
int do_getsysinfo(void);

/* mount.c */
int fs_mountpoint_o(void);
int fs_mountpoint_s(void);
int fs_readsuper_o(void);
int fs_readsuper_s(void);
int do_mount(void);
int do_umount(void);
int unmount(dev_t dev);

/* open.c */
int fs_create_o(void);
int fs_create_s(void);
int fs_mkdir_o(void);
int fs_mkdir_s(void);
int fs_mknod_o(void);
int fs_mknod_s(void);
int fs_slink_o(void);
int fs_slink_s(void);
int fs_newnode(void);
int do_close(void);
int do_creat(void);
int do_lseek(void);
int do_mknod(void);
int do_mkdir(void);
int do_open(void);
int do_slink(void);

/* path.c */
int lookup_o(void);
int fs_lookup_s(void);
struct inode *advance_o(struct inode **dirp, char string[NAME_MAX]);
struct inode *advance_nocheck(struct inode **dirp, char string[NAME_MAX]);
int search_dir(struct inode *ldir_ptr, char string [NAME_MAX], ino_t *numb, int flag);
int search_dir_nocheck(struct inode *ldir_ptr, char string [NAME_MAX], ino_t *numb, int flag);
struct inode *eat_path_o(char *path);
struct inode *last_dir_o(char *path, char string [NAME_MAX]);
struct inode *parse_path_o(char *path, char string[NAME_MAX], int action);

/* protect.c */
int fs_access_o(void);
int do_access(void);
int do_chmod(void);
int do_chown(void);
int do_umask(void);
int forbidden(struct inode *rip, mode_t access_desired);
int read_only(struct inode *ip);

/* read.c */
int fs_breadwrite_o(void);
int fs_breadwrite_s(void);
int fs_readwrite_o(void);
int fs_readwrite_s(void);
int do_read(void);
struct buf *rahead(struct inode *rip, block_t baseblock, u64_t position, unsigned bytes_ahead);
void read_ahead(void);
block_t read_map(struct inode *rip, off_t pos);
int read_write(int rw_flag);
zone_t rd_indir(struct buf *bp, int index);

/* stadir.c */
int do_chdir(void);
int do_fchdir(void);
int do_chroot(void);
int do_fstat(void);
int do_stat(void);
int do_fstatfs(void);
int do_rdlink(void);
int do_lstat(void);

/* super.c */
bit_t alloc_bit(struct super_block *sp, int map, bit_t origin);
void free_bit(struct super_block *sp, int map, bit_t bit_returned);
struct super_block *get_super(dev_t dev);
int mounted(struct inode *rip);
int read_super(struct super_block *sp);
int get_block_size(dev_t dev);

/* time.c */
int do_stime(void);
int do_utime(void);

/* utility.c */
time_t clock_time(void);
unsigned conv2(int norm, int w);
long conv4(int norm, long x);
int fetch_name(char *path, int len, int flag);
int no_sys(void);
int isokendpt_f(char *f, int l, int e, int *p, int ft);
void mfs_nul_f(char *file, int line, char *str, int len, int maxlen);
int mfs_min_f(char *file, int line, int len1, int len2);
void sanitycheck(char *file, int line);

#define SANITYCHECK sanitycheck(__FILE__, __LINE__)

#define okendpt(e, p) isokendpt_f(__FILE__, __LINE__, (e), (p), 1)
#define isokendpt(e, p) isokendpt_f(__FILE__, __LINE__, (e), (p), 0)

/* write.c */
void clear_zone(struct inode *rip, off_t pos, int flag);
int do_write(void);
struct buf *new_block(struct inode *rip, off_t position);
void zero_block(struct buf *bp);
int write_map(struct inode *, off_t, zone_t, int);
