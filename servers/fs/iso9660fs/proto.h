#ifndef __SERVERS_ISO9660FS_PROTO_H
#define __SERVERS_ISO9660FS_PROTO_H

#include "type.h"

/* Function prototypes for iso9660 file system. */

struct dir_record;
struct ext_attr_rec;
struct iso9660_vd_pri;

int fs_getnode(void);
int fs_putnode(void);
int fs_new_driver(void);
int fs_sync(void);
int lookup(void);
int fs_access(void);
int fs_getdents(void);
int fs_getdents_o(void);

/* main.c */
int main(void);
void reply(int who, message *m_out);

/* device.c */
int block_dev_io(int op, dev_t dev, int proc, void *buf,
			       u64_t pos, int bytes, int flags);
int dev_open(endpoint_t driver_e, dev_t dev, int proc,
			   int flags);
void dev_close(endpoint_t driver_e, dev_t dev);

/* super.c */
int release_v_pri(struct iso9660_vd_pri *v_pri);
int read_vds(struct iso9660_vd_pri *v_pri, dev_t dev);
int create_v_pri(struct iso9660_vd_pri *v_pri, char *buffer,unsigned long address);

/* inode.c */
int release_dir_record(struct dir_record *dir);
struct dir_record *get_free_dir_record(void);
struct dir_record *get_dir_record(ino_t id_dir);
struct ext_attr_rec *get_free_ext_attr(void);
int create_ext_attr(struct ext_attr_rec *ext,
				char *buffer);
int create_dir_record(struct dir_record *dir, char *buffer,
				  u32_t address);
struct dir_record *load_dir_record_from_disk(u32_t address);

/* path.c */
int fs_lookup_s(void);
struct dir_record *advance(struct dir_record **dirp,
				       char string[NAME_MAX]);
 int search_dir(struct dir_record *ldir_ptr,
			     char string [NAME_MAX], ino_t *numb);
 struct dir_record *parse_path(char *path,
					    char string[NAME_MAX], 
                                            int action);

/* read.c */
int fs_read_s(void);
int fs_read(void);
int fs_bread(void);
int fs_bread_s(void);
int read_chunk(struct dir_record *dir, u64_t position,
			   unsigned off, int chunk, char *buff, int seg, 
			   int usr, int block_size, int *completed);

/* utility.c */
int no_sys(void);
void panic(char *who, char *mess, int num);

/* cache.c */
struct buf *get_block(block_t block);
void put_block(struct buf *bp);

/* ids.c */
/* void hash_init, (void)); */
/* int assign_id_to_dir_record, (struct dir_record *dir)); */
/* struct dir_record *get_dir_record_by_id,(int id)); */

/* mount.c */
int fs_readsuper(void);
int fs_readsuper_s(void);
int fs_mountpoint_s(void);
int fs_unmount(void);

/* stadir.c */
int fs_stat(void);
int fs_fstatfs(void);

#endif /* __SERVERS_ISO9660FS_PROTO_H */
