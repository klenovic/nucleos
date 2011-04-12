#ifndef __SERVERS_FS_PIPEFS_INODE_H
#define __SERVERS_FS_PIPEFS_INODE_H

/* Inode table.  This table holds inodes that are currently in use. */
#include <nucleos/queue.h>

struct pipe_inode {
  mode_t i_mode;		/* file type, protection, etc. */
  nlink_t i_nlinks;		/* how many links to this file */
  uid_t i_uid;			/* user id of the file's owner */
  gid_t i_gid;			/* group number */
  off_t i_size;			/* current file size in bytes */
  time_t i_atime;		/* time of last access (V2 only) */
  time_t i_mtime;		/* when was file data last changed */
  time_t i_ctime;		/* when was inode itself changed (V2 only)*/

#if defined(__KERNEL__) || defined(__UKERNEL__)
  /* The following items are not present on the disk. */
  dev_t i_dev;			/* which device is the inode on */
  ino_t i_num;			/* inode number on its (minor) device */
  int i_count;			/* # times inode used; 0 means slot is free */
  char i_update;		/* the ATIME, CTIME, and MTIME bits are here */

  LIST_ENTRY(pipe_inode) i_hash;     /* hash list */
  TAILQ_ENTRY(pipe_inode) i_unused;  /* free and unused list */
#endif /* defined(__KERNEL__) || defined(__UKERNEL__) */
};

#if defined(__KERNEL__) || defined(__UKERNEL__)
extern struct pipe_inode inode[];

/* list of unused/free inodes */ 
extern TAILQ_HEAD(unused_inodes_t, pipe_inode) unused_inodes;

/* inode hashtable */
extern LIST_HEAD(inodelist, pipe_inode) hash_inodes[];

#define NIL_INODE (struct pipe_inode *) 0	/* indicates absence of inode slot */

#endif /* defined(__KERNEL__) || defined(__UKERNEL__) */

#endif /* __SERVERS_FS_PIPEFS_INODE_H */
