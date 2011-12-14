/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*      boot.c - Load and start Minix.      Author: Kees J. Bot
 *                    27 Dec 1991
 */
#include <stdio.h>
#include <stdlib.h>
#include <nucleos/kernel.h>
#include <nucleos/stringify.h>
#include <nucleos/stddef.h>
#include <nucleos/types.h>
#include <nucleos/stat.h>
#include <nucleos/limits.h>
#include <nucleos/string.h>
#include <nucleos/errno.h>
#include <nucleos/type.h>
#include <nucleos/com.h>
#include <nucleos/dmap.h>
#include <nucleos/const.h>
#include <nucleos/minlib.h>
#include <nucleos/syslib.h>
#include <nucleos/video.h>
#include <nucleos/param.h>
#include <kernel/const.h>
#include <kernel/types.h>
#include <ibm/partition.h>
#include <ibm/bios.h>
#include "rawfs.h"
#include "boot.h"

#define arraysize(a)      (sizeof(a) / sizeof((a)[0]))
#define arraylimit(a)     ((a) + arraysize(a))
#define between(a, c, z)  ((unsigned) ((c) - (a)) <= ((z) - (a)))

u16_t vid_port;                /* Video i/o port. */
u32_t vid_mem_base;    /* Video memory base address. */
u32_t vid_mem_size;    /* Video memory size. */

int fsok= -1;     /* File system state.  Initially unknown. */

static int block_size;

/* this data is reserved for BIOS int 0x13 to put the 'specification packet'
 * in. It has a structure of course, but we don't define a struct because
 * of compiler padding. We fiddle out the bytes ourselves later.
 */
unsigned char boot_spec[24];

char *bios_err(int err)
/* Translate BIOS error code to a readable string.  (This is a rare trait
 * known as error checking and reporting.  Take a good look at it, you won't
 * see it often.)
 */
{
  static struct errlist {
    int     err;
    char    *what;
  } errlist[] = {
    { 0x00, "No error" },
    { 0x01, "Invalid command" },
    { 0x02, "Address mark not found" },
    { 0x03, "Disk write-protected" },
    { 0x04, "Sector not found" },
    { 0x05, "Reset failed" },
    { 0x06, "Floppy disk removed" },
    { 0x07, "Bad parameter table" },
    { 0x08, "DMA overrun" },
    { 0x09, "DMA crossed 64 KB boundary" },
    { 0x0A, "Bad sector flag" },
    { 0x0B, "Bad track flag" },
    { 0x0C, "Media type not found" },
    { 0x0D, "Invalid number of sectors on format" },
    { 0x0E, "Control data address mark detected" },
    { 0x0F, "DMA arbitration level out of range" },
    { 0x10, "Uncorrectable CRC or ECC data error" },
    { 0x11, "ECC corrected data error" },
    { 0x20, "Controller failed" },
    { 0x40, "Seek failed" },
    { 0x80, "Disk timed-out" },
    { 0xAA, "Drive not ready" },
    { 0xBB, "Undefined error" },
    { 0xCC, "Write fault" },
    { 0xE0, "Status register error" },
    { 0xFF, "Sense operation failed" }
  };
  struct errlist *errp;

  for (errp= errlist; errp < arraylimit(errlist); errp++) {
    if (errp->err == err) return errp->what;
  }
  return "Unknown error";
}

char *unix_err(int err)
/* Translate the few errors rawfs can give. */
{
  switch (err) {
  case ENOENT:    return "No such file or directory";
  case ENOTDIR:   return "Not a directory";
  default:  return "Unknown error";
  }
}

void rwerr(char *rw, off_t sec, int err)
{
  printf("\n%s error 0x%02x (%s) at sector %ld absolute\n",
    rw, err, bios_err(err), sec);
}

void readerr(off_t sec, int err)  { rwerr("Read", sec, err); }
void writerr(off_t sec, int err)  { rwerr("Write", sec, err); }

void readblock(off_t blk, char *buf, int block_size)
/* Read blocks for the rawfs package. */
{
  int r;
  u32_t sec = lowsec + blk * RATIO(block_size);

  if(!block_size) {
    printf("block_size 0\n");
    exit(1);
  }

  if ((r= readsectors(mon2abs(buf), sec, 1 * RATIO(block_size))) != 0) {
    readerr(sec, r); 
    exit(1);
  }
}


int activate;

struct biosdev {
  char name[8];
  int device, primary, secondary;
} bootdev, tmpdev;

int get_master(char *master, struct part_entry **table, u32_t pos)
/* Read a master boot sector and its partition table. */
{
  int r, n;
  struct part_entry *pe, **pt;

  if ((r = readsectors(mon2abs(master), pos, 1)) != 0)
    return r;

  pe = (struct part_entry *) (master + PART_TABLE_OFF);

  for (pt= table; pt < table + NR_PARTITIONS; pt++) 
    *pt= pe++;

  /* DOS has the misguided idea that partition tables must be sorted. */
  if (pos != 0) /* But only the primary. */
    return 0;

  n= NR_PARTITIONS;
  do {
    for (pt= table; pt < table + NR_PARTITIONS-1; pt++) {
      if (pt[0]->sysind == NO_PART
          || pt[0]->lowsec > pt[1]->lowsec) {
        pe= pt[0]; pt[0]= pt[1]; pt[1]= pe;
      }
    }
  } while (--n > 0);

  return 0;
}

void initialize(void)
{
  char master[SECTOR_SIZE];
  struct part_entry *table[NR_PARTITIONS];
  int r, p;
  u32_t masterpos;

  /* Copy the boot program to the far end of low memory, this must be
   * done to get out of the way of Minix, and to put the data area
   * cleanly inside a 64K chunk if using BIOS I/O (no DMA problems).
   */
  u32_t oldaddr = caddr;
  u32_t memend = mem[0].base + mem[0].size;
  u32_t newaddr = (memend - runsize) & ~0x0000FL;

  u32_t dma64k = (memend - 1) & ~0x0FFFFL;

  /* Check if data segment crosses a 64K boundary. */
  if (newaddr + (daddr - caddr) < dma64k) {
    newaddr= (dma64k - runsize) & ~0x0000FL;
  }

  /* Set the new caddr for relocate. */
  caddr = newaddr;
  /* Copy code and data. */
  raw_copy(newaddr, oldaddr, runsize);

  /* Make the copy running. */
  relocate();

  /* Take the monitor out of the memory map if we have memory to spare,
   * and also keep the BIOS data area safe (1.5K), plus a bit extra for
   * where we may have to put a.out headers for older kernels.
   */
  if (mem[1].size > 512*1024L) {
    mem[0].size = newaddr;
  }

  mem[0].base += 2048;
  mem[0].size -= 2048;

  /* Find out what the boot device and partition was. */
  bootdev.name[0]= 0;
  bootdev.device= device;
  bootdev.primary= -1;
  bootdev.secondary= -1;


  if (device < 0x80) {
    /* Floppy. */
    strcpy(bootdev.name, "fd0");
    bootdev.name[2] += bootdev.device;
    return;
  }

  /* Disk: Get the partition table from the very first sector, and
   * determine the partition we booted from using the information from
   * the booted partition entry as passed on by the bootstrap (rem_part).
   * All we need from it is the partition offset.
   */
  raw_copy(mon2abs(&lowsec),
    vec2abs(&rem_part) + offsetof(struct part_entry, lowsec),
    sizeof(lowsec));

  masterpos= 0;   /* Master bootsector position. */

  for (;;) {
    /* Extract the partition table from the master boot sector. */
    if ((r= get_master(master, table, masterpos)) != 0) {
      readerr(masterpos, r); 

      exit(1);
    }

    /* See if you can find "lowsec" back. */
    for (p= 0; p < NR_PARTITIONS; p++) {
      if (lowsec - table[p]->lowsec < table[p]->size) {
        break;
      }
    }

    if (lowsec == table[p]->lowsec) {       /* Found! */
      if (bootdev.primary < 0)
        bootdev.primary= p;
      else
        bootdev.secondary= p;
      break;
    }

    if (p == NR_PARTITIONS || bootdev.primary >= 0
          || table[p]->sysind != MINIX_PART) {
      /* The boot partition cannot be named, this only means
       * that "bootdev" doesn't work.
       */
      bootdev.device= -1;
      return;
    }

    /* See if the primary partition is subpartitioned. */
    bootdev.primary= p;
    masterpos= table[p]->lowsec;
  }

  strcpy(bootdev.name, "d0p0");

  bootdev.name[1] += (device - 0x80);
  bootdev.name[3] += bootdev.primary;
  if (bootdev.secondary >= 0) {
    strcat(bootdev.name, "s0");
    bootdev.name[5] += bootdev.secondary;
  }
  /* Find out about the video hardware. */
  raw_copy(mon2abs(&vid_port), VDU_CRT_BASE_ADDR, sizeof(vid_port));
  if(vid_port == C_6845) {
    vid_mem_base = COLOR_BASE;
    vid_mem_size = COLOR_SIZE;
  } else {
    vid_mem_base = MONO_BASE;
    vid_mem_size = MONO_SIZE;
  }

  if(get_video() >= 3)
    vid_mem_size = EGA_SIZE;
}

void sfree(char *s)
/* Free a non-null string. */
{
  if (s != 0) free(s);
}

char *copystr(char *s)
/* Copy a non-null string using malloc. */
{
  char *c;

  if (*s == 0)
     return "";

  c = malloc((strlen(s) + 1) * sizeof(char));
  strcpy(c, s);

  return c;
}

int is_default(environment *e)
{
  return (e->flags & E_SPECIAL) && e->defval == 0;
}

environment** searchenv(char *name)
{
  environment **aenv= &env;

  while (*aenv != 0 && strcmp((*aenv)->name, name) != 0) {
    aenv= &(*aenv)->next;
  }

  return aenv;
}

#define b_getenv(name)  (*searchenv(name))
/* Return the environment *structure* belonging to name, or 0 if not found. */

char *b_value(char *name)
/* The value of a variable. */
{
  environment *e= b_getenv(name);

  return e == 0 || !(e->flags & E_VAR) ? 0 : e->value;
}

char *b_body(char *name)
/* The value of a function. */
{
  environment *e= b_getenv(name);

  return e == 0 || !(e->flags & E_FUNCTION) ? 0 : e->value;
}

int b_setenv(int flags, char *name, char *arg, char *value)
/* Change the value of an environment variable.  Returns the flags of the
 * variable if you are not allowed to change it, 0 otherwise.
 */
{
  environment **aenv, *e;

  if (*(aenv= searchenv(name)) == 0) {
    e = malloc(sizeof(*e));
    e->name = copystr(name);
    e->flags = flags;
    e->defval = 0;
    e->next = 0;
    *aenv = e;
  } else {
    e= *aenv;

    /* Don't change special variables to functions or vv. */
    if (e->flags & E_SPECIAL
      && (e->flags & E_FUNCTION) != (flags & E_FUNCTION)
    ) return e->flags;

    e->flags = (e->flags & E_STICKY) | flags;
    if (is_default(e)) {
      e->defval = e->value;
    } else {
      sfree(e->value);
    }
    sfree(e->arg);
  }

  e->arg = copystr(arg);
  e->value = copystr(value);

  return 0;
}

int b_setvar(int flags, char *name, char *value)
/* Set variable or simple function. */
{
  int r;

  if((r=b_setenv(flags, name, "", value))) {
    return r;
  }

  return r;
}

long a2l(char *a)
/* Cheap atol(). */
{
  int sign= 1;
  long n= 0;

  if (*a == '-') { sign= -1; a++; }

  while (between('0', *a, '9')) n= n * 10 + (*a++ - '0');

  return sign * n;
}

char *ul2a(u32_t n, unsigned b)
/* Transform a long number to ascii at base b, (b >= 8). */
{
  static char num[(CHAR_BIT * sizeof(n) + 2) / 3 + 1];
  char *a= arraylimit(num) - 1;
  static char hex[16] = "0123456789ABCDEF";

  do *--a = hex[(int) (n % b)]; while ((n/= b) > 0);
  return a;
}

char *ul2a10(u32_t n)
/* Transform a long number to ascii at base 10. */
{
  return ul2a(n, 10);
}

unsigned a2x(char *a)
/* Ascii to hex. */
{
  unsigned n= 0;
  int c;

  for (;;) {
    c= *a;
    if (between('0', c, '9')) c= c - '0' + 0x0;
    else
    if (between('A', c, 'F')) c= c - 'A' + 0xA;
    else
    if (between('a', c, 'f')) c= c - 'a' + 0xa;
    else
      break;
    n= (n<<4) | c;
    a++;
  }
  return n;
}

void get_parameters(void)
{
  char params[SECTOR_SIZE + 1];
  int processor;
  memory *mp;
  static char bus_type[][4] = {
    "xt", "at", "mca"
  };
  static char vid_type[][4] = {
    "mda", "cga", "ega", "ega", "vga", "vga"
  };
  static char vid_chrome[][6] = {
    "mono", "color"
  };

  memset(params,0,sizeof(params));

  /* Variables that Minix needs: */
  b_setvar(E_SPECIAL|E_VAR|E_DEV, "rootdev", "c0d0p0");
  b_setvar(E_SPECIAL|E_VAR|E_DEV, "ramimagedev", "c0d0p0");
  b_setvar(E_SPECIAL|E_VAR, "ramsize", "0");
  b_setvar(E_SPECIAL|E_VAR, "hz", __stringify(HZ));
  processor = getprocessor();

  if(processor == 1586)
    processor = 686;

  b_setvar(E_SPECIAL|E_VAR, "processor", ul2a10(processor));
  b_setvar(E_SPECIAL|E_VAR, "bus", bus_type[get_bus()]);
  b_setvar(E_SPECIAL|E_VAR, "video", vid_type[get_video()]);
  b_setvar(E_SPECIAL|E_VAR, "chrome", vid_chrome[get_video() & 1]);

  params[0]= 0;

  for (mp= mem; mp < arraylimit(mem); mp++) {
    if (mp->size == 0)
      continue;

    if (params[0] != 0) 
      strcat(params, ",");

    strcat(params, ul2a(mp->base, 0x10));
    strcat(params, ":");
    strcat(params, ul2a(mp->size, 0x10));
  }

  b_setvar(E_SPECIAL|E_VAR, "memory", params);

  return;
}

int numprefix(char *s, char **ps)
/* True iff s is a string of digits.  *ps will be set to the first nondigit
 * if non zero, otherwise the string should end.
 */
{
  char *n= s;

  while (between('0', *n, '9')) n++;

  if (n == s) return 0;

  if (ps == 0) return *n == 0;

  *ps= n;
  return 1;
}

int numeric(char *s)
{
  return numprefix(s, (char **) 0);
}

/* Device numbers of standard MINIX devices. */
#define DEV_FD0   0x0200
static dev_t dev_cNd0[] = { 0x0300, 0x0800, 0x0A00, 0x0C00, 0x1000 };
#define minor_p0s0   128

static int block_size;

dev_t name2dev(char *name)
/* Translate, say, /dev/c0d0p2 to a device number.  If the name can't be
 * found on the boot device, then do some guesswork.  The global structure
 * "tmpdev" will be filled in based on the name, so that "boot d1p0" knows
 * what device to boot without interpreting device numbers.
 */
{
  dev_t dev;
  ino_t ino;
  struct stat st;
  char *n, *s;

  /* "boot *d0p2" means: make partition 2 active before you boot it. */
  if ((activate= (name[0] == '*'))) name++;

  /* The special name "bootdev" must be translated to the boot device. */
  if (strcmp(name, "bootdev") == 0) {
    if (bootdev.device == -1) {
      printf("The boot device could not be named\n");
      errno= 0;
      return -1;
    }
    name= bootdev.name;
  }

  /* If our boot device doesn't have a file system, or we want to know
   * what a name means for the BIOS, then we need to interpret the
   * device name ourselves: "fd" = floppy, "c0d0" = hard disk, etc.
   */
  tmpdev.device= tmpdev.primary= tmpdev.secondary= -1;
  dev= -1;
  n= name;
  if (strncmp(n, "/dev/", 5) == 0) n+= 5;

  if (strcmp(n, "ram") == 0) {
    dev= DEV_RAM;
  } else
  if (n[0] == 'f' && n[1] == 'd' && numeric(n+2)) {
    /* Floppy. */
    tmpdev.device= a2l(n+2);
    dev= DEV_FD0 + tmpdev.device;
  } else
  if ((n[0] == 'h' || n[0] == 's') && n[1] == 'd' && numprefix(n+2, &s)
    && (*s == 0 || (between('a', *s, 'd') && s[1] == 0))
  ) {
    /* Old style hard disk (backwards compatibility.) */
    dev= a2l(n+2);
    tmpdev.device= dev / (1 + NR_PARTITIONS);
    tmpdev.primary= (dev % (1 + NR_PARTITIONS)) - 1;
    if (*s != 0) {
      /* Subpartition. */
      tmpdev.secondary= *s - 'a';
      dev= minor_p0s0
        + (tmpdev.device * NR_PARTITIONS
          + tmpdev.primary) * NR_PARTITIONS
        + tmpdev.secondary;
    }
    tmpdev.device+= 0x80;
    dev+= n[0] == 'h' ? dev_cNd0[0] : dev_cNd0[2];
  } else {
    /* Hard disk. */
    int ctrlr= 0;

    if (n[0] == 'c' && between('0', n[1], '4')) {
      ctrlr= (n[1] - '0');
      tmpdev.device= 0;
      n+= 2;
    }
    if (n[0] == 'd' && between('0', n[1], '7')) {
      tmpdev.device= (n[1] - '0');
      n+= 2;
      if (n[0] == 'p' && between('0', n[1], '3')) {
        tmpdev.primary= (n[1] - '0');
        n+= 2;
        if (n[0] == 's' && between('0', n[1], '3')) {
          tmpdev.secondary= (n[1] - '0');
          n+= 2;
        }
      }
    }
    if (*n == 0) {
      dev= dev_cNd0[ctrlr];
      if (tmpdev.secondary < 0) {
        dev += tmpdev.device * (NR_PARTITIONS+1)
          + (tmpdev.primary + 1);
      } else {
        dev += minor_p0s0
          + (tmpdev.device * NR_PARTITIONS
              + tmpdev.primary) * NR_PARTITIONS
          + tmpdev.secondary;
      }
      tmpdev.device+= 0x80;
    }
  }

  /* Look the name up on the boot device for the UNIX device number. */
  if (fsok == -1) fsok= r_super(&block_size) != 0;
  if (fsok) {
    /* The current working directory is "/dev". */
    ino= r_lookup(r_lookup(ROOT_INO, "dev"), name);

    if (ino != 0) {
      /* Name has been found, extract the device number. */
      r_stat(ino, &st);
      if (!S_ISBLK(st.st_mode)) {
        printf("%s is not a block device\n", name);
        errno= 0;
        return (dev_t) -1;
      }
      dev= st.st_rdev;
    }
  }

  if (tmpdev.primary < 0) activate= 0;    /* Careful now! */

  if (dev == -1) {
    printf("Can't recognize '%s' as a device\n", name);
    errno= 0;
  }
  return dev;
}

void boot(void)
{
	/* Initialize tables. */
	initialize();

	/* Get environment variables from the parameter sector. */
	get_parameters();

	if (boot_nucleos() < 0)
		printf("Error while booting kernel\n");

	/* @nucleos: only in case of error */
	while (1) halt_cpu();
}
