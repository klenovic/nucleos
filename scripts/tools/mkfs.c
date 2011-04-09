/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* mkfs  -  make the MINIX filesystem   Authors: Tanenbaum et al.
 * Authors: Andy Tanenbaum, Paul Ogilvie, Frans Meulenbroeks, Bruce Evans
 *
 * This program can make version 1, 2 and 3 file systems, as follows:
 *    mkfs /dev/fd0 1200    # Version 3 (default)
 *    mkfs -1 /dev/fd0 360  # Version 1
 *
 * Note that the version 1 and 2 file systems produced by this program are not
 * compatible with the original version 1 and 2 file system layout.
 */
#define _LARGEFILE64_SOURCE
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <linux/fs.h>
#include <linux/hdreg.h>
#include <sys/ioctl.h>

/* some minix specific stuffs */
#include "include/const.h"
#include "include/a.out.h"
#include "include/tools.h"
#include "include/dirent.h"
#include "include/fs/const.h"
#include "include/fs/type.h"
#include "include/fs/super.h"

#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#define INODE_MAP   2
#define MAX_TOKENS  10
#define LINE_LEN    200
#define BIN       2
#define BINGRP    2
#define BIT_MAP_SHIFT 13

#define INODE_MAX   ((unsigned) 65535)

#define SRCNAME "mkfs.c"
/* tracing */
#define TRACE(msg,args...) \
  fprintf(stderr,"%s:%s:%d:",SRCNAME,__FUNCTION__,__LINE__); \
  fprintf(stderr, msg, ##args );

extern char *optarg;
extern int optind;

int next_zone, next_inode, zone_size, zone_shift = 0, zoff;
MNX(block_t) nrblocks;
int inode_offset, lct = 0, disk, fd, print = 0, file = 0;
unsigned int nrinodes;
int override = 0, simple = 0, dflag;
int donttest;           /* skip test if it fits on medium */
char *progname;

long current_time, bin_time;
char *zero, *lastp;
char *umap_array;      /* bit map tells if block read yet */
int umap_array_elements = 0;
MNX(block_t) zone_map;         /* where is zone map? (depends on # inodes) */
int inodes_per_block;
int fs_version;
unsigned int block_size;

FILE *proto;
int use_proto = 0;

MNX(block_t) sizeup(char *device);
void super(MNX(zone_t) zones, MNX(Ino_t) inodes);
void rootdir(MNX(Ino_t) inode);
void eat_dir(MNX(Ino_t) parent);
void eat_file(MNX(Ino_t) inode, int f);
void enter_dir(MNX(Ino_t) parent, char *name, MNX(Ino_t) child);
void incr_size(MNX(Ino_t) n, long count);
static MNX(ino_t) alloc_inode(int mode, int usrid, int grpid);
static MNX(zone_t) alloc_zone(void);
void add_zone(MNX(Ino_t) n, MNX(zone_t) z, long bytes, long cur_time);
void add_z_1(MNX(Ino_t) n, MNX(zone_t) z, long bytes, long cur_time);
void add_z_2(MNX(Ino_t) n, MNX(zone_t) z, long bytes, long cur_time);
void incr_link(MNX(Ino_t) n);
void insert_bit(MNX(block_t) block, int bit);
int mode_con(char *p);
static void get_line(char line[LINE_LEN], char *parse[MAX_TOKENS]);
long file_time(int f);
void pexit(char *s);
void copy(char *from, char *to, int count);
void print_fs(void);
int read_and_set(MNX(block_t) n);
void special(char *string);
void get_block(MNX(block_t) n, char *buf);
void get_super_block(char *buf);
void put_block(MNX(block_t) n, char *buf);
void cache_init(void);
void flush(void);
void mx_read(int blocknr, char *buf);
void mx_write(int blocknr, char *buf);
void dexit(char *s, int sectnum, int err);
void usage(void);
char *alloc_block(void);

/* Convert from bit count to a block count. The usual expression
 *
 *    (nr_bits + (1 << BITMAPSHIFT) - 1) >> BITMAPSHIFT
 *
 * doesn't work because of overflow.
 *
 * Other overflow bugs, such as the expression for N_ILIST overflowing when
 * s_inodes is just over V*_INODES_PER_BLOCK less than the maximum+1, are not
 * fixed yet, because that number of inodes is silly.
 */
/* The above comment doesn't all apply now bit_t is long.  Overflow is now
 * unlikely, but negative bit counts are now possible (though unlikely)
 * and give silly results.
 */
static int bitmapsize(MNX(bit_t) nr_bits, int block_size)
{
  int nr_blocks;

  nr_blocks = (int) (nr_bits / FS_BITS_PER_BLOCK(block_size));
  if (((MNX(bit_t)) nr_blocks * FS_BITS_PER_BLOCK(block_size)) < nr_bits) ++nr_blocks;
  return(nr_blocks);
}

/*================================================================
 *          mkfs  -  make filesystem
 *===============================================================*/
int main(int argc, char *argv[])
{
  int nread, mode, usrid, grpid, ch;
  MNX(block_t) blocks, maxblocks;
  MNX(block_t) i;
  MNX(ino_t) root_inum;
  MNX(ino_t) inodes;
  MNX(zone_t) zones;
  char *token[MAX_TOKENS], line[LINE_LEN];
  struct stat statbuf;

  /* Get two times, the current time and the mod time of the binary of
   * mkfs itself.  When the -d flag is used, the later time is put into
   * the i_mtimes of all the files.  This feature is useful when
   * producing a set of file systems, and one wants all the times to be
   * identical. First you set the time of the mkfs binary to what you
   * want, then go.
   */
  current_time = time((time_t *) 0);  /* time mkfs is being run */
  stat(argv[0], &statbuf);
  bin_time = statbuf.st_mtime;  /* time when mkfs binary was last modified */

  /* Process switches. */
  progname = argv[0];
  blocks = 0;
  i = 0;
  fs_version = 3;
  inodes_per_block = 0;
  block_size = 0;
  while ((ch = getopt(argc, argv, "12b:di:lotpB:")) != EOF)
    switch (ch) {
      case '1':
        fs_version = 1;
        inodes_per_block = V1_INODES_PER_BLOCK;
        break;
      case '2':
        fs_version = 2;
        break;
      case 'b':
        blocks = strtoul(optarg, (char **) NULL, 0);
        break;
      case 'd':
        dflag = 1;
        current_time = bin_time;
        break;
      case 'i':
        i = strtoul(optarg, (char **) NULL, 0);
        break;
      case 'l':   print = 1;    break;
      case 'o':   override = 1;   break;
      case 'p':   use_proto = 1;   break;
      case 't':   donttest = 1;   break;
      case 'B':   block_size = atoi(optarg);    break;
      default:  usage();
    }

  if (argc == optind) usage();

  if(fs_version == 3) {
    if(!block_size) block_size = _MAX_BLOCK_SIZE; /* V3 default block size */
    if(block_size%SECTOR_SIZE || block_size < _MIN_BLOCK_SIZE) {
        fprintf(stderr, "block size must be multiple of sector (%d) "
            "and at least %d bytes\n",
            SECTOR_SIZE, _MIN_BLOCK_SIZE);
        pexit("specified block size illegal");
    }
    if(block_size%V2_INODE_SIZE) {
        TRACE("block_size=%d(\n",block_size);
        fprintf(stderr, "block size must be a multiple of inode size (%d bytes)\n",
            V2_INODE_SIZE);
        pexit("specified block size illegal");
    }
  } else {
    if(block_size) {
        pexit("Can't specify a block size if FS version is <3");
    }
    block_size = _STATIC_BLOCK_SIZE;    /* V1/V2 block size */
  }

  if(!inodes_per_block)
    inodes_per_block = V2_INODES_PER_BLOCK(block_size);

  /* now that the block size is known, do buffer allocations where
   * possible.
   */
  zero = alloc_block();
  bzero(zero, block_size);

  /* Determine the size of the device if not specified as -b or proto. */
  maxblocks = sizeup(argv[optind]);
  if (argc - optind == 1 && blocks == 0) {
    blocks = maxblocks;
    /* blocks == 0 is checked later, but leads to a funny way of
     * reporting a 0-sized device (displays usage).
     */
    if(blocks < 1) {
        fprintf(stderr, "%s: zero size device.\n", progname);
        return 1;
    }
  }

  /* The remaining args must be 'special proto', or just 'special' if the
   * no. of blocks has already been specified.
   */
  if (argc - optind != 2 && (argc - optind != 1 || blocks == 0)) usage();

  if (blocks > maxblocks) {
    fprintf(stderr, "%s: %s: number of blocks too large for device.\n",
        progname, argv[optind]);
    return 1;
  }
  /* Check and start processing proto. */
  optarg = argv[++optind];
  if (optind < argc && (proto = fopen(optarg, "r")) != NULL) {
  /* Prototype file is readable. */
  lct = 1;
    get_line(line, token);   /* skip boot block info */

    /* Read the line with the block and inode counts. */
    get_line(line, token);
    blocks = atol(token[0]);
    inodes = atoi(token[1]);

    /* Process mode line for root directory. */
    get_line(line, token);
    mode = mode_con(token[0]);
    usrid = atoi(token[1]);
    grpid = atoi(token[2]);
  } else {
    lct = 0;
    if (optind < argc) {
        /* Maybe the prototype file is just a size.  Check. */
        blocks = strtoul(optarg, (char **) NULL, 0);
        if (blocks == 0) pexit("Can't open prototype file");
    }
    if (i == 0) {
        i = blocks / 2;
        if (blocks >= 20000) i = blocks / 3;
        if (blocks >= 40000) i = blocks / 4;
        if (blocks >= 60000) i = blocks / 5;
        if (blocks >= 80000) i = blocks / 6;
        if (blocks >= 100000) i = blocks / 7;

        /* round up to fill inode block */
        i += inodes_per_block - 1;
        i = i / inodes_per_block * inodes_per_block;
        if (i > INODE_MAX && fs_version < 3) i = INODE_MAX;

    }
    if (blocks < 5) pexit("Block count too small");
  
    if (i < 1) pexit("Inode count too small");
    if (i > INODE_MAX && fs_version < 3) pexit("Inode count too large");
    inodes = (MNX(ino_t)) i;

    /* Make simple file system of the given size, using defaults. */
    mode = 040777;
    usrid = BIN;
    grpid = BINGRP;
    simple = 1;
  }

  nrblocks = blocks;
  nrinodes = inodes;

  {
    size_t bytes;
    bytes = 1 + blocks/8;
    if (!(umap_array = malloc(bytes))) {
      fprintf(stderr, "mkfs: can't allocate block bitmap (%d bytes).\n", bytes);
      exit(1);
    }
    umap_array_elements = bytes;
  }


  /* Open special. */
  special(argv[--optind]);

  if (!donttest) {
    short *testb;
    ssize_t w;

    testb = (short *) alloc_block();

    /* Try writing the last block of partition or diskette. */
//    if(lseek64(fd, /*(off64_t)*/((blocks - 1) * block_size), SEEK_SET) < 0) {
    if(lseek(fd, (off_t)((blocks - 1) * block_size), SEEK_SET) < 0) {
        TRACE("blocks=%ld,block_size=%d\n",blocks,block_size);
        pexit("couldn't seek to last block to test size (1)");
    }
    testb[0] = 0x3245;
    testb[1] = 0x11FF;
    testb[block_size/2-1] = 0x1F2F;
    if ((w=write(fd, (char *) testb, block_size)) != block_size) {
        if(w < 0) perror("write");
        printf("%d/%d\n", w, block_size);
        pexit("File system is too big for minor device (write)");
    }
    sync();         /* flush write, so if error next read fails */
    if(lseek64(fd, (off64_t)((blocks - 1) * block_size), SEEK_SET) < 0) {
        pexit("couldn't seek to last block to test size (2)");
    }
    testb[0] = 0;
    testb[1] = 0;
    nread = read(fd, (char *) testb, block_size);
    if (nread != block_size || testb[0] != 0x3245 || testb[1] != 0x11FF ||
        testb[block_size/2-1] != 0x1F2F) {
        if(nread < 0) perror("read");
          printf("nread = %d\n", nread);
          printf("testb = 0x%x 0x%x 0x%x\n", testb[0], testb[1], testb[block_size-1]);
        pexit("File system is too big for minor device (read)");

    }
    lseek64(fd, (off64_t)((blocks - 1) * block_size), SEEK_SET);
    testb[0] = 0;
    testb[1] = 0;
    if (write(fd, (char *) testb, block_size) != block_size)
        pexit("File system is too big for minor device (write2)");
    lseek(fd, 0L, SEEK_SET);
    free(testb);
  }

  /* Make the file-system */

  cache_init();

    put_block((MNX(block_t)) 0, zero);   /* Write a null boot block. */

  zone_shift = 0;         /* for future use */
  zones = nrblocks >> zone_shift;

  super(zones, inodes);

  root_inum = alloc_inode(mode, usrid, grpid);
  rootdir(root_inum);
  if (simple == 0) eat_dir(root_inum);

  if (print) print_fs();
  flush();
  return(0);

  /* NOTREACHED */
}                 /* end main */


/*================================================================
 *          sizeup  -  determine device size
 *===============================================================*/
MNX(block_t) sizeup(char *device)
{
	int fd;
	MNX(u64_t) devsize64;
	MNX(block_t) d;
	struct stat st;
	unsigned int rem;
	MNX(u64_t) resize;

	if ((fd = open(device, O_RDONLY)) == -1) {
		if (errno != ENOENT)
			perror("sizeup open");
			return 0;
	}

	if(!use_proto) {
		if (ioctl(fd, BLKGETSIZE64, &devsize64) != 0) {
			fprintf(stderr,"mkfs: %d: %s: can't device size.\n", __LINE__, __FUNCTION__);
			return 0;
		}

		close(fd);

		/* cast into unsigned long */
		d = (unsigned long)(devsize64 / block_size);
		rem = (unsigned int)(devsize64 % block_size);
		resize = (MNX(u64_t))(d*block_size + rem);

		if(resize != devsize64) {
			d = ULONG_MAX;
			fprintf(stderr, "mkfs: truncating FS at %lu blocks\n", d);
		}
	} else {
		off_t size = lseek(fd, 0, SEEK_END);

		if (size == (off_t) -1) {
			fprintf(stderr, "mkfs: %d: %s: can't device size.\n", __LINE__, __FUNCTION__);
			return 0;
		}

		d = size / block_size;
	}

	close(fd);
	return d;
}


/*================================================================
 *         super  -  construct a superblock
 *===============================================================*/

void super(zones, inodes)
MNX(zone_t) zones;
MNX(ino_t) inodes;
{
  unsigned int i;
  int inodeblks;
  int initblks;
  MNX(u32_t) nb;

  MNX(zone_t) v1sq, v2sq;
  MNX(zone_t) zo;
  struct MNX(super_block) *sup;
  char *buf, *cp;

  buf = alloc_block();

  for (cp = buf; cp < &buf[block_size]; cp++) *cp = 0;
  sup = (struct MNX(super_block) *) buf;    /* lint - might use a union */

  sup->s_ninodes = inodes;
  if (fs_version == 1) {
    sup->s_nzones = zones;
    if (sup->s_nzones != zones) pexit("too many zones");
  } else {
    sup->s_nzones = 0;  /* not used in V2 - 0 forces errors early */
    sup->s_zones = zones;
  }

#define BIGGERBLOCKS "Please try a larger block size for an FS of this size.\n"
  sup->s_imap_blocks = nb = bitmapsize((MNX(bit_t)) (1 + inodes), block_size);
  if(sup->s_imap_blocks != nb) {
       fprintf(stderr, "mkfs: too many inode bitmap blocks.\n" BIGGERBLOCKS);
       exit(1);
  }
  sup->s_zmap_blocks = nb = bitmapsize((MNX(bit_t)) zones, block_size);
  if(nb != sup->s_zmap_blocks) {
       fprintf(stderr, "mkfs: too many block bitmap blocks.\n" BIGGERBLOCKS);
       exit(1);
  }

  inode_offset = sup->s_imap_blocks + sup->s_zmap_blocks + 2;
  inodeblks = (inodes + inodes_per_block - 1) / inodes_per_block;
  initblks = inode_offset + inodeblks;

  sup->s_firstdatazone = nb = (initblks + (1 << zone_shift) - 1) >> zone_shift;

  if(nb >= zones) pexit("bit maps too large");
  if(nb != sup->s_firstdatazone) {
       fprintf(stderr, "mkfs: too much bitmap and inode data.\n" BIGGERBLOCKS);
       exit(1);
  }

  zoff = sup->s_firstdatazone - 1;
  sup->s_log_zone_size = zone_shift;

  if (fs_version == 1) {
    sup->s_magic = SUPER_MAGIC;   /* identify super blocks */
    v1sq = (MNX(zone_t)) V1_INDIRECTS * V1_INDIRECTS;
    zo = V1_NR_DZONES + (long) V1_INDIRECTS + v1sq;
    sup->s_max_size = zo * block_size;
  } else {
    v2sq = (MNX(zone_t)) V2_INDIRECTS(block_size) * V2_INDIRECTS(block_size);
    zo = V2_NR_DZONES + (MNX(zone_t)) V2_INDIRECTS(block_size) + v2sq;
    if(fs_version == 2) {
        sup->s_magic = SUPER_V2;/* identify super blocks */
        sup->s_max_size = zo * block_size;
    } else {
        sup->s_magic = SUPER_V3;
        sup->s_block_size = block_size;
        sup->s_disk_version = 0;
#define MAX_MAX_SIZE  ((unsigned long) LONG_MAX)
        if(MAX_MAX_SIZE/block_size < zo) {
            sup->s_max_size = MAX_MAX_SIZE;
        }
        else {
            sup->s_max_size = zo * block_size;
        }
    }
  }

  zone_size = 1 << zone_shift;  /* nr of blocks per zone */

  if (lseek(fd, (off_t) _STATIC_BLOCK_SIZE, SEEK_SET) == (off_t) -1) {
    pexit("super() couldn't seek");
  }
  if (write(fd, buf, _STATIC_BLOCK_SIZE) != _STATIC_BLOCK_SIZE) {
    pexit("super() couldn't write");
  }

  /* Clear maps and inodes. */
  for (i = 2; i < initblks; i++) 
    put_block((MNX(block_t)) i, zero);

  next_zone = sup->s_firstdatazone;
  next_inode = 1;

  zone_map = INODE_MAP + sup->s_imap_blocks;

  insert_bit(zone_map, 0);    /* bit zero must always be allocated */
  insert_bit((MNX(block_t)) INODE_MAP, 0);   /* inode zero not used but
                     * must be allocated */

  free(buf);
}


/*================================================================
 *        rootdir  -  install the root directory
 *===============================================================*/
void rootdir(inode)
MNX(ino_t) inode;
{
  MNX(zone_t) z;

  z = alloc_zone();
  add_zone(inode, z, 2 * sizeof(struct MNX(direct)), current_time);
  enter_dir(inode, ".", inode);
  enter_dir(inode, "..", inode);
  incr_link(inode);
  incr_link(inode);
}


/*================================================================
 *      eat_dir  -  recursively install directory
 *===============================================================*/
void eat_dir(parent)
MNX(ino_t) parent;
{
  /* Read prototype lines and set up directory. Recurse if need be. */
  char *token[MAX_TOKENS], *p;
  char line[LINE_LEN];
  int mode, usrid, grpid, maj, min, f;
  MNX(ino_t) n;
  MNX(zone_t) z;
  long size;

  while (1) {
    get_line(line, token);
    p = token[0];
    if (*p == '$') return;
    p = token[1];
    mode = mode_con(p);
    usrid = atoi(token[2]);
    grpid = atoi(token[3]);

    if (grpid & 0200)
      fprintf(stderr, "A.S.Tanenbaum\n");

    n = alloc_inode(mode, usrid, grpid);

    /* Enter name in directory and update directory's size. */
    enter_dir(parent, token[0], n);
    incr_size(parent, sizeof(struct MNX(direct)));

    /* Check to see if file is directory or special. */
    incr_link(n);

    if (*p == 'd') {
        /* This is a directory. */
        z = alloc_zone();     /* zone for new directory */
        add_zone(n, z, 2 * sizeof(struct MNX(direct)), current_time);
        enter_dir(n, ".", n);
        enter_dir(n, "..", parent);
        incr_link(parent);
        incr_link(n);
        eat_dir(n);
    } else if (*p == 'b' || *p == 'c') {
        /* Special file. */
        maj = atoi(token[4]);
        min = atoi(token[5]);
        size = 0;

        if (token[6]) 
          size = atoi(token[6]);

        size = block_size * size;
        add_zone(n, (MNX(zone_t)) ((maj << 8) | min), size, current_time);
    } else {
        /* Regular file. Go read it. */
        if ((f = open(token[4], O_RDONLY)) < 0) {
            fprintf(stderr, "%s: Can't open %s: %s\n",
                progname, token[4], strerror(errno));
        } else {
            eat_file(n, f);
        }
    }
  }

}

/*================================================================
 *        eat_file  -  copy file to MINIX
 *===============================================================*/
/* Zonesize >= blocksize */
void eat_file(inode, f)
MNX(ino_t) inode;
int f;
{
  int ct = 0, i, j, k;
  MNX(zone_t) z = 0;
  char *buf;
  long timeval;

  buf = alloc_block();

  do {
    for (i = 0, j = 0; i < zone_size; i++, j += ct) {
        for (k = 0; k < block_size; k++) 
          buf[k] = 0;
        if ((ct = read(f, buf, block_size)) > 0) {
            if (i == 0) 
              z = alloc_zone();
            put_block((z << zone_shift) + i, buf);
        }
    }

    timeval = (dflag ? current_time : file_time(f));

    if (ct)
      add_zone(inode, z, (long) j, timeval);
  } while (ct == block_size);
  close(f);
}



/*================================================================
 *      directory & inode management assist group
 *===============================================================*/
void enter_dir(parent, name, child)
MNX(ino_t) parent, child;
char *name;
{
  /* Enter child in parent directory */
  /* Works for dir > 1 block and zone > block */
  int i, j, k, l, off;
  MNX(block_t) b;
  MNX(zone_t) z;
  char *p1, *p2;
  struct MNX(direct) *dir_entry;
  MNX(d1_inode) ino1[V1_INODES_PER_BLOCK];
  MNX(d2_inode) *ino2;
  int nr_dzones;

  b = ((parent - 1) / inodes_per_block) + inode_offset;
  off = (parent - 1) % inodes_per_block;

  if(!(dir_entry = malloc(NR_DIR_ENTRIES(block_size) * sizeof(*dir_entry))))
    pexit("couldn't allocate directory entry");

  if(!(ino2 = malloc(V2_INODES_PER_BLOCK(block_size) * sizeof(*ino2))))
    pexit("couldn't allocate block of inodes entry");

  if (fs_version == 1) {
    get_block(b, (char *) ino1);
    nr_dzones = V1_NR_DZONES;
  } else {
    get_block(b, (char *) ino2);
    nr_dzones = V2_NR_DZONES;
  }
  for (k = 0; k < nr_dzones; k++) {
    if (fs_version == 1) {
        z = ino1[off].d1_zone[k];
        if (z == 0) {
            z = alloc_zone();
            ino1[off].d1_zone[k] = z;
        }
    } else {
        z = ino2[off].d2_zone[k];
        if (z == 0) {
            z = alloc_zone();
            ino2[off].d2_zone[k] = z;
        }
    }
    for (l = 0; l < zone_size; l++) {
        get_block((z << zone_shift) + l, (char *) dir_entry);
        for (i = 0; i < NR_DIR_ENTRIES(block_size); i++) {
            if (dir_entry[i].d_ino == 0) {
                dir_entry[i].d_ino = child;
                p1 = name;
                p2 = dir_entry[i].d_name;
                j = sizeof(dir_entry[i].d_name);
                while (j--) {
                    *p2++ = *p1;
                    if (*p1 != 0) p1++;
                }
                put_block((z << zone_shift) + l, (char *) dir_entry);
                if (fs_version == 1) {
                    put_block(b, (char *) ino1);
                } else {
                    put_block(b, (char *) ino2);
                }
                free(dir_entry);
                free(ino2);
                return;
            }
        }
    }
  }

  printf("Directory-inode %ld beyond direct blocks.  Could not enter %s\n",
     parent, name);
  pexit("Halt");
}


void add_zone(n, z, bytes, cur_time)
MNX(ino_t) n;
MNX(zone_t) z;
long bytes, cur_time;
{
  if (fs_version == 1) {
    add_z_1(n, z, bytes, cur_time);
  } else {
    add_z_2(n, z, bytes, cur_time);
  }
}

void add_z_1(n, z, bytes, cur_time)
MNX(ino_t) n;
MNX(zone_t) z;
long bytes, cur_time;
{
  /* Add zone z to inode n. The file has grown by 'bytes' bytes. */

  int off, i;
  MNX(block_t) b;
  MNX(zone_t) indir;
  MNX(zone1_t) blk[V1_INDIRECTS];
  MNX(d1_inode) *p;
  MNX(d1_inode) inode[V1_INODES_PER_BLOCK];

  b = ((n - 1) / V1_INODES_PER_BLOCK) + inode_offset;
  off = (n - 1) % V1_INODES_PER_BLOCK;
  get_block(b, (char *) inode);
  p = &inode[off];
  p->d1_size += bytes;
  p->d1_mtime = cur_time;

  for (i = 0; i < V1_NR_DZONES; i++)
    if (p->d1_zone[i] == 0) {
        p->d1_zone[i] = (MNX(zone1_t)) z;
        put_block(b, (char *) inode);
        return;
    }
  put_block(b, (char *) inode);

  /* File has grown beyond a small file. */
  if (p->d1_zone[V1_NR_DZONES] == 0)
    p->d1_zone[V1_NR_DZONES] = (MNX(zone1_t)) alloc_zone();
  indir = p->d1_zone[V1_NR_DZONES];
  put_block(b, (char *) inode);
  b = indir << zone_shift;
  get_block(b, (char *) blk);
  for (i = 0; i < V1_INDIRECTS; i++)
    if (blk[i] == 0) {
        blk[i] = (MNX(zone1_t)) z;
        put_block(b, (char *) blk);
        return;
    }
  pexit("File has grown beyond single indirect");
}

void add_z_2(n, z, bytes, cur_time)
MNX(ino_t) n;
MNX(zone_t) z;
long bytes, cur_time;
{
  /* Add zone z to inode n. The file has grown by 'bytes' bytes. */

  int off, i;
  MNX(block_t) b;
  MNX(zone_t) indir;
  MNX(zone_t) *blk;
  MNX(d2_inode) *p;
  MNX(d2_inode) *inode;

  if(!(blk = malloc(V2_INDIRECTS(block_size)*sizeof(*blk))))
    pexit("Couldn't allocate indirect block");

  if(!(inode = malloc(V2_INODES_PER_BLOCK(block_size)*sizeof(*inode))))
    pexit("Couldn't allocate block of inodes");

  b = ((n - 1) / V2_INODES_PER_BLOCK(block_size)) + inode_offset;
  off = (n - 1) % V2_INODES_PER_BLOCK(block_size);
  get_block(b, (char *) inode);
  p = &inode[off];
  p->d2_size += bytes;
  p->d2_mtime = cur_time;

  for (i = 0; i < V2_NR_DZONES; i++)
    if (p->d2_zone[i] == 0) {
        p->d2_zone[i] = z;
        put_block(b, (char *) inode);
        free(blk);
        free(inode);
        return;
    }

  put_block(b, (char *) inode);

  /* File has grown beyond a small file. */
  if (p->d2_zone[V2_NR_DZONES] == 0) 
    p->d2_zone[V2_NR_DZONES] = alloc_zone();

  indir = p->d2_zone[V2_NR_DZONES];
  put_block(b, (char *) inode);
  b = indir << zone_shift;
  get_block(b, (char *) blk);

  for (i = 0; i < V2_INDIRECTS(block_size); i++)
    if (blk[i] == 0) {
        blk[i] = z;
        put_block(b, (char *) blk);
        free(blk);
        free(inode);
        return;
    }
  pexit("File has grown beyond single indirect");
}


void incr_link(n)
MNX(ino_t) n;
{
  /* Increment the link count to inode n */
  int off;
  static int enter = 0;
  MNX(block_t) b;

  if(enter) 
    exit(1);

  b = ((n - 1) / inodes_per_block) + inode_offset;
  off = (n - 1) % inodes_per_block;

  if (fs_version == 1) {
    MNX(d1_inode) inode1[V1_INODES_PER_BLOCK];

    get_block(b, (char *) inode1);
    inode1[off].d1_nlinks++;
    put_block(b, (char *) inode1);
  } else {
    static MNX(d2_inode) *inode2 = NULL;
    int n;

    n = sizeof(*inode2) * V2_INODES_PER_BLOCK(block_size);
    if(!inode2 && !(inode2 = malloc(n)))
        pexit("couldn't allocate a block of inodes");

    get_block(b, (char *) inode2);
    inode2[off].d2_nlinks++;
    put_block(b, (char *) inode2);
  }
  enter = 0;
}


void incr_size(n, count)
MNX(ino_t) n;
long count;
{
  /* Increment the file-size in inode n */
  MNX(block_t) b;
  int off;

  b = ((n - 1) / inodes_per_block) + inode_offset;
  off = (n - 1) % inodes_per_block;
  if (fs_version == 1) {
    MNX(d1_inode) inode1[V1_INODES_PER_BLOCK];

    get_block(b, (char *) inode1);
    inode1[off].d1_size += count;
    put_block(b, (char *) inode1);
  } else {
    MNX(d2_inode) *inode2;
    if(!(inode2 = malloc(V2_INODES_PER_BLOCK(block_size) * sizeof(*inode2))))
        pexit("couldn't allocate a block of inodes");

    get_block(b, (char *) inode2);
    inode2[off].d2_size += count;
    put_block(b, (char *) inode2);
    free(inode2);
  }
}


/*================================================================
 *           allocation assist group
 *===============================================================*/
static MNX(ino_t) alloc_inode(mode, usrid, grpid)
int mode, usrid, grpid;
{
  MNX(ino_t) num;
  int off;
  MNX(block_t) b;

  num = next_inode++;

  if (num > nrinodes) {
    fprintf(stderr, "have %d inodoes\n", nrinodes);
    pexit("File system does not have enough inodes");
  }

  b = ((num - 1) / inodes_per_block) + inode_offset;
  off = (num - 1) % inodes_per_block;

  if (fs_version == 1) {
    MNX(d1_inode) inode1[V1_INODES_PER_BLOCK];

    get_block(b, (char *) inode1);
    inode1[off].d1_mode = mode;
    inode1[off].d1_uid = usrid;
    inode1[off].d1_gid = grpid;
    put_block(b, (char *) inode1);
  } else {
    MNX(d2_inode) *inode2;

    if(!(inode2 = malloc(V2_INODES_PER_BLOCK(block_size) * sizeof(*inode2))))
        pexit("couldn't allocate a block of inodes");

    get_block(b, (char *) inode2);
    inode2[off].d2_mode = mode;
    inode2[off].d2_uid = usrid;
    inode2[off].d2_gid = grpid;
    put_block(b, (char *) inode2);

    free(inode2);
  }

  /* Set the bit in the bit map. */
  /* DEBUG FIXME.  This assumes the bit is in the first inode map block. */
  insert_bit((MNX(block_t)) INODE_MAP, (int) num);
  return(num);
}


static MNX(zone_t) alloc_zone()
{
  /* Allocate a new zone */
  /* Works for zone > block */
  MNX(block_t) b;
  int i;
  MNX(zone_t) z;

  z = next_zone++;
  b = z << zone_shift;

  if ((b + zone_size) > nrblocks)
    pexit("File system not big enough for all the files");

  for (i = 0; i < zone_size; i++)
    put_block(b + i, zero); /* give an empty zone */

  /* DEBUG FIXME.  This assumes the bit is in the first zone map block. */
  insert_bit(zone_map, (int) (z - zoff));     /* lint, NOT OK because
                         * z hasn't been broken
                         * up into block +
                         * offset yet. */
  return(z);
}


void insert_bit(block, bit)
MNX(block_t) block;
int bit;
{
  /* Insert 'count' bits in the bitmap */
  int w, s;
  short *buf;

  buf = (short *) alloc_block();

  if (block < 0) 
    pexit("insert_bit called with negative argument");

  get_block(block, (char *) buf);
  w = bit / (8 * sizeof(short));
  s = bit % (8 * sizeof(short));
  buf[w] |= (1 << s);
  put_block(block, (char *) buf);

  free(buf);
}


/*================================================================
 *        proto-file processing assist group
 *===============================================================*/
int mode_con(p)
char *p;
{
  /* Convert string to mode */
  int o1, o2, o3, mode;
  char c1, c2, c3;

  c1 = *p++;
  c2 = *p++;
  c3 = *p++;
  o1 = *p++ - '0';
  o2 = *p++ - '0';
  o3 = *p++ - '0';

  mode = (o1 << 6) | (o2 << 3) | o3;

  if (c1 == 'd') 
    mode += I_DIRECTORY;

  if (c1 == 'b') 
    mode += I_BLOCK_SPECIAL;

  if (c1 == 'c') 
    mode += I_CHAR_SPECIAL;

  if (c1 == '-') 
    mode += I_REGULAR;

  if (c2 == 'u') 
    mode += I_SET_UID_BIT;

  if (c3 == 'g') 
    mode += I_SET_GID_BIT;

  return(mode);
}

static void get_line(char line[LINE_LEN], char *parse[MAX_TOKENS])
{
  /* Read a line and break it up in tokens */
  int k;
  char c, *p;
  int d;

  for (k = 0; k < MAX_TOKENS; k++) 
    parse[k] = 0;

  for (k = 0; k < LINE_LEN; k++) 
    line[k] = 0;

  k = 0;
  parse[0] = 0;
  p = line;

  while (1) {
    if (++k > LINE_LEN) 
      pexit("Line too long");

    d = fgetc(proto);

    if (d == EOF) 
      pexit("Unexpected end-of-file");

    *p = d;

    if (*p == '\n') 
      lct++;

    if (*p == ' ' || *p == '\t') 
      *p = 0;

    if (*p == '\n') {
        *p++ = 0;
        *p = '\n';
        break;
    }

    p++;
  }

  k = 0;
  p = line;
  lastp = line;
  while (1) {
    c = *p++;

    if (c == '\n') 
      return;

    if (c == 0) 
      continue;

    parse[k++] = p - 1;

    do {
        c = *p++;
    } while (c != 0 && c != '\n');
  }
}


#if 0
/*================================================================
 *            other stuff
 *===============================================================*/
void check_mtab(devname)
char *devname;          /* /dev/hd1 or whatever */
{
/* Check to see if the special file named in s is mounted. */

  int n, r;
  struct stat sb;
  char special[PATH_MAX + 1], mounted_on[PATH_MAX + 1], version[10], rw_flag[10];

  r= stat(devname, &sb);
  if (r == -1)
  {
    if (errno == ENOENT)
        return; /* Does not exist, and therefore not mounted. */
    fprintf(stderr, "%s: stat %s failed: %s\n",
        progname, devname, strerror(errno));
    exit(1);
  }
  if (!S_ISBLK(sb.st_mode))
  {
    /* Not a block device and therefore not mounted. */
    return;
  }

  if (load_mtab("mkfs") < 0) return;
  while (1) {
    n = get_mtab_entry(special, mounted_on, version, rw_flag);
    if (n < 0) return;
    if (strcmp(devname, special) == 0) {
        /* Can't mkfs on top of a mounted file system. */
        fprintf(stderr, "%s: %s is mounted on %s\n",
            progname, devname, mounted_on);
        exit(1);
    }
  }
}
#endif

long file_time(f)
int f;
{
  struct stat statbuf;
  fstat(f, &statbuf);

  return(statbuf.st_mtime);
}


void pexit(s)
char *s;
{
  fprintf(stderr, "%s: %s\n", progname, s);

  if (lct != 0)
    fprintf(stderr, "Line %d being processed when error detected.\n", lct);

  flush();

  exit(2);
}


void copy(from, to, count)
char *from, *to;
int count;
{
  while (count--) *to++ = *from++;
}

char *alloc_block()
{
  char *buf;

  if(!(buf = malloc(block_size))) {
    pexit("couldn't allocate filesystem buffer");
  }

  bzero(buf, block_size);

  return buf;
}

void print_fs()
{
  int i, j;
  MNX(ino_t) k;
  MNX(d1_inode) inode1[V1_INODES_PER_BLOCK];
  MNX(d2_inode) *inode2;
  unsigned short *usbuf;
  MNX(block_t) b, inode_limit;
  struct MNX(direct) *dir;

  if(!(inode2 = malloc(V2_INODES_PER_BLOCK(block_size) * sizeof(*inode2))))
    pexit("couldn't allocate a block of inodes");

  if(!(dir = malloc(NR_DIR_ENTRIES(block_size)*sizeof(*dir))))
    pexit("malloc of directory entry failed");

  usbuf = (unsigned short *) alloc_block();

  get_super_block((char *) usbuf);
  printf("\nSuperblock: ");

  for (i = 0; i < 8; i++) 
    printf("%06o ", usbuf[i]);

  get_block((MNX(block_t)) 2, (char *) usbuf);
  printf("...\nInode map:  ");

  for (i = 0; i < 9; i++) 
    printf("%06o ", usbuf[i]);

  get_block((MNX(block_t)) 3, (char *) usbuf);
  printf("...\nZone  map:  ");

  for (i = 0; i < 9; i++) 
    printf("%06o ", usbuf[i]);

  printf("...\n");

  free(usbuf);
  usbuf = NULL;

  k = 0;
  for (b = inode_offset; k < nrinodes; b++) {
    if (fs_version == 1) {
        get_block(b, (char *) inode1);
    } else {
        get_block(b, (char *) inode2);
    }
    for (i = 0; i < inodes_per_block; i++) {
        k = inodes_per_block * (int) (b - inode_offset) + i + 1;
        /* Lint but OK */
        if (k > nrinodes) 
          break;

        if (fs_version == 1) {
            if (inode1[i].d1_mode != 0) {
                printf("Inode %2ld:  mode=", k);
                printf("%06o", inode1[i].d1_mode);
                printf("  uid=%2d  gid=%2d  size=",
                inode1[i].d1_uid, inode1[i].d1_gid);
                printf("%6ld", inode1[i].d1_size);
                printf("  zone[0]=%d\n", inode1[i].d1_zone[0]);
            }

            if ((inode1[i].d1_mode & I_TYPE) == I_DIRECTORY) {
                /* This is a directory */
                get_block(inode1[i].d1_zone[0], (char *) dir);
                for (j = 0; j < NR_DIR_ENTRIES(block_size); j++)
                    if (dir[j].d_ino)
                        printf("\tInode %2ld: %s\n", dir[j].d_ino, dir[j].d_name);
            }
        } else {
            if (inode2[i].d2_mode != 0) {
                printf("Inode %2ld:  mode=", k);
                printf("%06o", inode2[i].d2_mode);
                printf("  uid=%2d  gid=%2d  size=",
                inode2[i].d2_uid, inode2[i].d2_gid);
                printf("%6ld", inode2[i].d2_size);
                printf("  zone[0]=%ld\n", inode2[i].d2_zone[0]);
            }

            if ((inode2[i].d2_mode & I_TYPE) == I_DIRECTORY) {
                /* This is a directory */
                get_block(inode2[i].d2_zone[0], (char *) dir);
                for (j = 0; j < NR_DIR_ENTRIES(block_size); j++)
                    if (dir[j].d_ino)
                        printf("\tInode %2ld: %s\n", dir[j].d_ino, dir[j].d_name);
            }
        }
    }
  }

  printf("%d inodes used.   %d zones used.\n", next_inode - 1, next_zone);
  free(dir);
  free(inode2);
}


int read_and_set(n)
MNX(block_t) n;
{
/* The first time a block is read, it returns all 0s, unless there has
 * been a write.  This routine checks to see if a block has been accessed.
 */

  int w, s, mask, r;

  w = n / 8;

  if(w >= umap_array_elements) {
       pexit("umap array too small - this can't happen");
  }

  s = n % 8;
  mask = 1 << s;
  r = (umap_array[w] & mask ? 1 : 0);
  umap_array[w] |= mask;

  return(r);
}

void usage()
{
  fprintf(stderr,
      "Usage: %s [-12dlot] [-b blocks] [-i inodes] [-B blocksize] special [proto]\n",
      progname);
  exit(1);
}

/*================================================================
 *          get_block & put_block for UNIX
 *===============================================================*/

void special(string)
char *string;
{
  fd = creat(string, 0777);
  close(fd);
  fd = open(string, O_RDWR);

  if (fd < 0) 
    pexit("Can't open special file");

  return;
}

void get_block(n, buf)
MNX(block_t) n;
char *buf;
{
/* Read a block. */

  int k;

  /* First access returns a zero block */
  if (read_and_set(n) == 0) {
    copy(zero, buf, block_size);
    return;
  }

  lseek64(fd, (off64_t)(n * block_size), SEEK_SET);
  k = read(fd, buf, block_size);

  if (k != block_size) {
    pexit("get_block couldn't read");
  }

  return;
}

void get_super_block(buf)
char *buf;
{
/* Read a block. */

  int k;

  if(lseek(fd, (off_t) SUPER_BLOCK_BYTES, SEEK_SET) < 0) {
    perror("lseek");
    pexit("seek failed");
  }

  k = read(fd, buf, _STATIC_BLOCK_SIZE);

  if (k != _STATIC_BLOCK_SIZE) {
    pexit("get_super_block couldn't read");
  }

  return;
}

void put_block(n, buf)
MNX(block_t) n;
char *buf;
{
/* Write a block. */

  (void) read_and_set(n);

  /* XXX - check other lseeks too. */
  if (lseek64(fd, (off64_t)(n * block_size), SEEK_SET) == -1) {
    pexit("put_block couldn't seek");
  }

  if (write(fd, buf, block_size) != block_size) {
    pexit("put_block couldn't write");
  }

  return;
}


/* Dummy routines to keep source file clean from #ifdefs */

void flush()
{
  return;
}

void cache_init()
{
  return;
}
