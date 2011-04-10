/* Inode table.  This table holds inodes that are currently in use.  In some
 * cases they have been opened by an open() or creat() system call, in other
 * cases the file system itself needs the inode for one reason or another,
 * such as to search a directory for a path name.
 * The first part of the struct holds fields that are present on the
 * disk; the second part holds fields not present on the disk.
 * The disk inode part is also declared in "type.h" as 'd_inode'
 *
 */

#ifndef __SERVERS_EXT2_INODE_H
#define __SERVERS_EXT2_INODE_H

#include <nucleos/queue.h>
#include <servers/ext2/type.h>

/* Disk part of inode structure was taken from
 * linux/include/linux/ext2_fs.h.
 */
struct inode {
	__u16 i_mode;	/* File mode */
	__u16 i_uid;	/* Low 16 bits of Owner Uid */
	__u32 i_size;	/* Size in bytes */
	__u32 i_atime;	/* Access time */
	__u32 i_ctime;	/* Creation time */
	__u32 i_mtime;	/* Modification time */
	__u32 i_dtime;	/* Deletion Time */
	__u16 i_gid;	/* Low 16 bits of Group Id */
	__u16 i_links_count;	/* Links count */
	__u32 i_blocks;		/* 512-byte blocks count */
	__u32 i_flags;		/* File flags */

	union {
		struct {
			__u32 l_i_reserved1;
		} linux1;
		struct {
			__u32 h_i_translator;
		} hurd1;
		struct {
			__u32 m_i_reserved1;
		} masix1;
	} osd1;	/* OS dependent 1 */

	__u32 i_block[EXT2_N_BLOCKS];	/* Pointers to blocks */
	__u32 i_generation;		/* File version (for NFS) */
	__u32 i_file_acl;		/* File ACL */
	__u32 i_dir_acl;		/* Directory ACL */
	__u32 i_faddr;			/* Fragment address */

	union {
		struct {
			__u8 l_i_frag;	/* Fragment number */
			__u8 l_i_fsize;	/* Fragment size */
			__u16 i_pad1;
			__u16 l_i_uid_high;	/* these 2 fields    */
			__u16 l_i_gid_high;	/* were reserved2[0] */
			__u32 l_i_reserved2;
		} linux2;

		struct {
			__u8 h_i_frag;	/* Fragment number */
			__u8 h_i_fsize;	/* Fragment size */
			__u16 h_i_mode_high;
			__u16 h_i_uid_high;
			__u16 h_i_gid_high;
			__u32 h_i_author;
		} hurd2;
			struct {
			__u8 m_i_frag;	/* Fragment number */
			__u8 m_i_fsize;	/* Fragment size */
			__u16 m_pad1;
			__u32 m_i_reserved2[2];
		} masix2;
	} osd2;				/* OS dependent 2 */

#if defined(__KERNEL__) || defined(__UKERNEL__)
	/* The following items are not present on the disk. */
	dev_t i_dev;			/* which device is the inode on */
	ino_t i_num;			/* inode number on its (minor) device */
	int i_count;			/* # times inode used; 0 means slot is free */
	struct ext2_super_block *i_sp;	/* pointer to super block for inode's device */
	char i_dirt;			/* CLEAN or DIRTY */
	block_t i_bsearch;		/* where to start search for new blocks,
					 * also this is last allocated block.
					 */
	off_t i_last_pos_bl_alloc;	/* last write position for which we allocated
					 * a new block (should be block i_bsearch).
					 * used to check for sequential operation.
					 */
	off_t i_last_dpos;		/* where to start dentry search */
	int i_last_dentry_size;	/* size of last found dentry */

	char i_mountpoint;	/* true if mounted on */

	char i_seek;		/* set on LSEEK, cleared on READ/WRITE */
	char i_update;		/* the ATIME, CTIME, and MTIME bits are here */

	block_t i_prealloc_blocks[EXT2_PREALLOC_BLOCKS];	/* preallocated blocks */
	int i_prealloc_count;	/* number of preallocated blocks */
	int i_prealloc_index;	/* index into i_prealloc_blocks */
	int i_preallocation;	/* use preallocation for this inode, normally
				 * it's reset only when non-sequential write
				 * happens.
				 */

	LIST_ENTRY(inode) i_hash;	/* hash list */
	TAILQ_ENTRY(inode) i_unused;	/* free and unused list */
#endif /* defined(__KERNEL__) || defined(__UKERNEL_) */

};

#if defined(__KERNEL__) || defined(__UKERNEL__)
extern struct inode inode[];

/* list of unused/free inodes */
extern TAILQ_HEAD(unused_inodes_t, inode) unused_inodes;

/* inode hashtable */
extern LIST_HEAD(inodelist, inode) hash_inodes[INODE_HASH_SIZE];

extern unsigned int inode_cache_hit;
extern unsigned int inode_cache_miss;

/* Field values.  Note that CLEAN and DIRTY are defined in "const.h" */
#define NO_SEEK		0	/* i_seek = NO_SEEK if last op was not SEEK */
#define ISEEK		1	/* i_seek = ISEEK if last op was SEEK */
#endif /* defined(__KERNEL__) || defined(__UKERNEL__) */

#endif /* __SERVER_EXT2_INODE_H */
