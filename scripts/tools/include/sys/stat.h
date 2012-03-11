/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The <sys/stat.h> header defines a struct that is used in the stat() and
 * fstat functions.  The information in this struct comes from the i-node of
 * some file.  These calls are the only approved way to inspect i-nodes.
 */

#ifndef __SYS_STAT_H
#define __SYS_STAT_H

#include "types.h"



struct mnx_stat {
  MNX(dev_t) st_dev;   /* major/minor device number */
  MNX(ino_t) st_ino;   /* i-node number */
  MNX(mode_t) st_mode; /* file mode, protection bits, etc. */
  short int st_nlink;  /* # links; TEMPORARY HACK: should be nlink_t*/
  MNX(uid_t) st_uid;   /* uid of the file's owner */
  short int st_gid;    /* gid; TEMPORARY HACK: should be gid_t */
  MNX(dev_t) st_rdev;
  MNX(off_t) st_size;   /* file size */
  MNX(time_t) st_atim; /* time of last access */
  MNX(time_t) st_mtim; /* time of last data modification */
  MNX(time_t) st_ctim; /* time of last file status change */
};

/* Traditional mask definitions for st_mode. */
#define MNX_S_IFMT  0170000 /* type of file */
#define MNX_S_IFLNK 0120000 /* symbolic link */
#define MNX_S_IFREG 0100000 /* regular */
#define MNX_S_IFBLK 0060000 /* block special */
#define MNX_S_IFDIR 0040000 /* directory */
#define MNX_S_IFCHR 0020000 /* character special */
#define MNX_S_IFIFO 0010000 /* this is a FIFO */
#define MNX_S_ISUID 0004000 /* set user id on execution */
#define MNX_S_ISGID 0002000 /* set group id on execution */
                            /* next is reserved for future use */
#define MNX_S_ISVTX   01000 /* save swapped text even after use */

/* POSIX masks for st_mode. */
#define MNX_S_IRWXU   00700 /* owner:  rwx------ */
#define MNX_S_IRUSR   00400 /* owner:  r-------- */
#define MNX_S_IWUSR   00200 /* owner:  -w------- */
#define MNX_S_IXUSR   00100 /* owner:  --x------ */

#define MNX_S_IRWXG   00070 /* group:  ---rwx--- */
#define MNX_S_IRGRP   00040 /* group:  ---r----- */
#define MNX_S_IWGRP   00020 /* group:  ----w---- */
#define MNX_S_IXGRP   00010 /* group:  -----x--- */

#define MNX_S_IRWXO   00007 /* others: ------rwx */
#define MNX_S_IROTH   00004 /* others: ------r-- */
#define MNX_S_IWOTH   00002 /* others: -------w- */
#define MNX_S_IXOTH   00001 /* others: --------x */

/* Synonyms for above. */
#define MNX_S_IEXEC		MNX_S_IXUSR
#define MNX_S_IWRITE	MNX_S_IWUSR
#define MNX_S_IREAD		MNX_S_IRUSR

/* The following macros test st_mode (from POSIX Sec. 5.6.1.1). */
#define MNX_S_ISREG(m)	(((m) & MNX_S_IFMT) == MNX_S_IFREG)	/* is a reg file */
#define MNX_S_ISDIR(m)	(((m) & MNX_S_IFMT) == MNX_S_IFDIR)	/* is a directory */
#define MNX_S_ISCHR(m)	(((m) & MNX_S_IFMT) == MNX_S_IFCHR)	/* is a char spec */
#define MNX_S_ISBLK(m)	(((m) & MNX_S_IFMT) == MNX_S_IFBLK)	/* is a block spec */
#define MNX_S_ISLNK(m)	(((m) & MNX_S_IFMT) == MNX_S_IFLNK)	/* is a symlink */
#define MNX_S_ISFIFO(m)	(((m) & MNX_S_IFMT) == MNX_S_IFIFO)	/* is a pipe/FIFO */

#endif /* _STAT_H */
