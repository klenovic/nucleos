/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* installboot 3.0 - Make a device bootable Author: Kees J. Bot
 *      21 Dec 1991
 *
 * Either make a device bootable or make an image from kernel, mm, fs, etc.
 */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <linux/fs.h>
#include <linux/hdreg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stddef.h>
#include "include/a.out.h"
#include "include/elfparse.h"
#include "include/sys/stat.h"
#include "include/elf.h"
#include "include/fs/rawfs.h"
#include "image.h"

#define BOOTBLOCK        0 /* Of course */
#define SECTOR_SIZE    512 /* Disk sector size. */
#define SIGNATURE   0xAA55 /* Boot block signature. */
#define BOOT_MAX        64 /* Absolute maximum size of secondary boot */
#define SIGPOS         510 /* Where to put signature word. */
#define PARTPOS        446 /* Offset to the partition table in a master boot block.*/

#define RATIO(b)   ((b)/SECTOR_SIZE)

#define between(a, c, z)  ((unsigned) ((c) - (a)) <= ((z) - (a)))
#define control(c)  between('\0', (c), '\37')

#define BOOT_BLOCK_SIZE 1024
#define _MAX_BLOCK_SIZE 4096
#define DO_TRACE 0

enum howto { FS, BOOT };

/* Every unrecognizied binaries are considered as RAW. */
enum binfmts {AOUT, ELF32, RAW};

void make_image_aout(char *image, char **procv);
void extract_image_aout(char *image);
void install_master_aout(char *device, char *masterboot, char **guide);
void dump_exec_elf32(MNX(exec_elf32_t)* e);

void dump_exec_elf32(MNX(exec_elf32_t)* e)
{
  printf("typetype=0x%x  ",e->type);
  printf("version=0x%x\n",e->version);
  printf("text_pstart=0x%x  ",e->text_pstart);
  printf("text_vstart=0x%x  ",e->text_vstart);
  printf("text_size=0x%x\n",e->text_size);
  printf("data_pstart=0x%x  ",e->data_pstart);
  printf("data_vstart=0x%x  ",e->data_vstart);
  printf("data_size=0x%x\n",e->data_size);
  printf("bss_pstart=0x%x  ",e->bss_pstart);
  printf("bss_vstart=0x%x  ",e->bss_vstart);
  printf("bss_size=0x%x\n",e->bss_size);
  printf("initial_ip=0x%x  ",e->initial_ip);
  printf("flags=0x%x  ",e->flags);
  printf("label=0x%x  ",e->label);
  printf("cmdline_offset=0x%x\n",e->cmdline_offset);

  return;
}

void report(char *label)
/* installboot: label: No such file or directory */
{
  NUCS_TRACE(DO_TRACE,"%s\n",label);
  fprintf(stderr, "installboot: %s: %s\n", label, strerror(errno));
}

void fatal(char *label)
{
  NUCS_TRACE(DO_TRACE,"%s\n",label);
  report(label);
  exit(1);
}

char *basename(char *name)
/* Return the last component of name, stripping trailing slashes from name.
 * Precondition: name != "/".  If name is prefixed by a label, then the
 * label is copied to the basename too.
 */
{
  NUCS_TRACE(DO_TRACE,"\n");
  static char base[IM_NAME_MAX];
  char *p, *bp= base;

  if ((p= strchr(name, ':')) != 0) {
    while (name <= p && bp < base + IM_NAME_MAX - 1)
      *bp++ = *name++;
  }

  for (;;) {
    if ((p= strrchr(name, '/')) == 0) {
      p= name; 
      break;
    }

    if (*++p != 0) 
      break;
    *--p= 0;
  }

  while (*p != 0 && bp < base + IM_NAME_MAX - 1)
    *bp++ = *p++;

  *bp= 0;

  return base;
}

void bread(FILE *f, char *name, void *buf, size_t len)
/* Read len bytes.  Don't dare return without them. */
{
  NUCS_TRACE(DO_TRACE,"\n");
  if (len > 0 && fread(buf, len, 1, f) != 1) {
    if (ferror(f))
       fatal(name);

    fprintf(stderr, "installboot: Unexpected EOF on %s\n", name);
    exit(1);
  }
}

void bwrite(FILE *f, char *name, void *buf, size_t len)
{
  NUCS_TRACE(DO_TRACE,"\n");
  if (len > 0 && fwrite(buf, len, 1, f) != 1) 
    fatal(name);
}

long total_text= 0, total_data= 0, total_bss= 0;
int making_image= 0;

/** 
 * Read file and fill image header 
 * @param[in] proc path to filename
 * @param[out] ihdr header to be filled
 * @return 0 if success otherwise -1
 */
int read_iheader_elf32(char* proc, struct MNX(image_header_elf32) *ihdr)
{
  NUCS_TRACE(DO_TRACE,"\n");
  elf32_ehdr_t ehdr;
  elf32_phdr_t* phdrs = 0;
  elf32_shdr_t* shdrs = 0;
  int i = 0;
  struct MNX(exec_elf32)* exec = &ihdr->process;
  char* buf;
  int fd_proc;
  struct stat st;           /* for linux */

  if(!proc) {
    fprintf(stderr,"Empty file name proc=%s\n",proc);
    return -1;
  }

  /* open boot code file on hosst for further examination */
  if ((fd_proc = open(proc, O_RDWR)) < 0) 
     fatal(proc);

  stat(proc, &st);
  buf = malloc(st.st_size*sizeof(char));

  if(!buf) {
    fprintf(stderr,"Not enough memory\n");
    exit(1);
  }

  read(fd_proc, buf, st.st_size);
  close(fd_proc);

  memset(ihdr, 0, sizeof(*ihdr));

  /* Put the basename of proc in the header. */
  strncpy(ihdr->name, basename(proc), IM_NAME_MAX);

  /* Read in the headers. */
  get_elf32_ehdr((char*)buf,&ehdr);
  phdrs = get_elf32_phdrs(buf, &ehdr, phdrs);
  shdrs = get_elf32_shdrs(buf, &ehdr, shdrs);

  free(buf);

#if DO_TRACE == 1
  printf("---[ ELF header ]---\n");
  dump_ehdr(&ehdr);
#endif

#if DO_TRACE == 1
  for(i=0; i<ehdr.e_phnum; i++) {
    if(phdrs) {
      printf("---[ %d. program header ]---\n",i);
      dump_phdr(&phdrs[i]);
    }
  }
#endif

#if DO_TRACE == 1
  for(i=0; i<ehdr.e_shnum; i++) {
    if(shdrs) {
      printf("---[ %d. section header ]---\n",i);
      dump_shdr(&shdrs[i]);
    }
  }
#endif

  // Initialize a local bootinfo record
  memset ((void*) exec, 0, sizeof (*exec));

  exec->type = NEXEC;
  exec->version = 1;
  exec->initial_ip = ehdr.e_entry;
  exec->offset_next = sizeof (*exec);
  exec->flags = ehdr.e_ident[4];  

  if (ehdr.e_phoff != 0 && ehdr.e_shoff == 0) {
    /*
     * We only have a program headers.  Walk all program headers
     * and try to figure out what the sections are.
     */

    for (i = 0; i < ehdr.e_phnum; i++) {
      elf32_phdr_t* ph = &phdrs[i];

      // Assume that a segment without write permissions is .text
      if ((ph->p_flags & PF_W) == 0) {
        exec->text_pstart = ph->p_paddr;
        exec->text_vstart = ph->p_paddr;
        exec->text_size   = ph->p_filesz;
      } else {
        exec->data_pstart = ph->p_paddr;
        exec->data_vstart = ph->p_paddr;
        exec->data_size   = ph->p_filesz;
      }

      if (ph->p_memsz > ph->p_filesz) {
        // Looks like a bss section
        exec->bss_pstart = ph->p_paddr + ph->p_filesz;
        exec->bss_vstart = ph->p_vaddr + ph->p_filesz;
        exec->bss_size   = ph->p_memsz - ph->p_filesz;
      }
    }

    return 0;
  }

  /*
   * If we have the section headers we can try to figure out the
   * real .text, .data, and .bss segments by inspecting the section
   * type and flags.
   */
  unsigned int tlow = ~0UL, thigh = 0;
  unsigned int dlow = ~0UL, dhigh = 0;
  unsigned int blow = ~0UL, bhigh = 0;

  for (i = 0; i < ehdr.e_shnum; i++) {
    elf32_shdr_t* sh = &shdrs[i];

    if (sh->sh_type == SHT_PROGBITS) {
      /* Also include read-only sections into .text */
      if ((sh->sh_flags & SHF_ALLOC) &&  
          ((sh->sh_flags & SHF_EXECINSTR) || (~sh->sh_flags & SHF_WRITE)))  {
        if (sh->sh_addr < tlow)
          tlow = sh->sh_addr;

        if (sh->sh_addr + sh->sh_size > thigh)
          thigh = sh->sh_addr + sh->sh_size;

      } else if ((sh->sh_flags & SHF_ALLOC) && (sh->sh_flags & SHF_WRITE)) {
      /* Other writable sections are counted as .data */
        if (sh->sh_addr < dlow)
          dlow = sh->sh_addr;

        if (sh->sh_addr + sh->sh_size > dhigh)
          dhigh = sh->sh_addr + sh->sh_size;
      }
    } else if (sh->sh_type == SHT_NOBITS) {
    /* Assume that all nobits sections are .bss */
      if (sh->sh_addr < blow)
        blow = sh->sh_addr;

      if (sh->sh_addr + sh->sh_size > bhigh)
        bhigh = sh->sh_addr + sh->sh_size;
    }
  }

  /*
   * Translate the virtual addresses of the sections to physical
   * addresses using the segments in the program header.
   */
#define INSEGMENT(a)    (a >= ph->p_vaddr && a < (ph->p_vaddr + ph->p_memsz))
#define PADDR(a)        (ph->p_paddr + a - ph->p_vaddr)

  for (i = 0; i < ehdr.e_phnum; i++) {
    elf32_phdr_t* ph = &phdrs[i];

    if (INSEGMENT (tlow)) {
      exec->text_pstart = PADDR (tlow);
      exec->text_vstart = tlow;
      exec->text_size = thigh - tlow;
    }

    if (INSEGMENT (dlow)) {
      exec->data_pstart = PADDR (dlow);
      exec->data_vstart = dlow;
      exec->data_size = dhigh - dlow;
    }

    if (INSEGMENT (blow)) {
      exec->bss_pstart = PADDR (blow);
      exec->bss_vstart = blow;
      exec->bss_size = bhigh - blow;
    }
  }

#if DO_TRACE == 1 
  dump_exec_elf32(exec);
#endif

  return 0;
}

int read_iheader_aout(int talk, char *proc, FILE *procf, struct MNX(image_header_aout) *ihdr)
/* Read the a.out header of a program and check it.  If procf happens to be
 * 0 then the header is already in *image_hdr and need only be checked.
 */
{
  NUCS_TRACE(DO_TRACE,"\n");
  int n, big= 0;
  static int banner= 0;
  struct MNX(exec) *phdr= &ihdr->process;

  if (procf == 0) {
    /* Header already present. */
    n= phdr->a_hdrlen;
  } else {
    memset(ihdr, 0, sizeof(*ihdr));

    /* Put the basename of proc in the header. */
    strncpy(ihdr->name, basename(proc), IM_NAME_MAX);

    /* Read the header. 
       @nucleos: FYI, this will move the pointer inside stream */
    n = fread(phdr, sizeof(char), A_MINHDR, procf);
    if (ferror(procf))
      fatal(proc);
  }

  if (n < A_MINHDR || BADMAG(*phdr)) {
    NUCS_TRACE(DO_TRACE,"n=%d A_MINHDR=%d || BADMAG=%d\n",n , A_MINHDR,BADMAG(*phdr))
    NUCS_TRACE(DO_TRACE,"a_magic0=0x%x a_magic1=0x%x\n",(*phdr).a_magic[0],(*phdr).a_magic[1]);
    fprintf(stderr, "installboot: %s is not an executable\n", proc);
    exit(1);
  }

  /* Get the rest of the exec header. */
  if (procf != 0) {
    bread(procf, proc, ((char *) phdr) + A_MINHDR, phdr->a_hdrlen - A_MINHDR);
  }

  if (talk && !banner) {
    printf(" %8s %8s %8s %9s\n","text","data","bss","size");
    banner= 1;
  }

  if (talk) {
    printf(" %8ld %8ld %8ld %9ld  %s\n", phdr->a_text, phdr->a_data,
                                         phdr->a_bss,
                                         phdr->a_text + phdr->a_data + phdr->a_bss,
                                         proc);
  }

  total_text+= phdr->a_text;
  total_data+= phdr->a_data;
  total_bss+= phdr->a_bss;

  if (phdr->a_cpu == A_I8086) {
    long data= phdr->a_data + phdr->a_bss;

    if (!(phdr->a_flags & A_SEP))
      data+= phdr->a_text;

    if (phdr->a_text >= 65536)
      big |= 1;
    if (data >= 65536)
      big |= 2;
  }

  if (big) {
    fprintf(stderr, "%s will crash, %s%s%s segment%s larger then 64K\n", proc,
                                                    (big & 1) ? "text" : "",
                                                    (big == 3) ? " and " : "",
                                                    (big & 2) ? "data" : "",
                                                    (big == 3) ? "s are" : " is");
  }

  return 0;
}

void padimage(char *image, FILE *imagef, int n)
/* Add n zeros to image to pad it to a sector boundary. */
{
  NUCS_TRACE(DO_TRACE,"\n");
  while (n > 0) {
    if (putc(0, imagef) == EOF) 
      fatal(image);
    n--;
  }
}

#define align(n)  (((n) + ((SECTOR_SIZE) - 1)) & ~((SECTOR_SIZE) - 1))

void copyexec(char *proc, FILE *procf, char *image, FILE *imagef, long n)
/* Copy n bytes from proc to image padded to fill a sector. */
{
  NUCS_TRACE(DO_TRACE,"\n");
  int pad, c;

  /* Compute number of padding bytes. */
  pad = align(n) - n;

  while (n > 0) {
    if ((c= getc(procf)) == EOF) {
      if (ferror(procf)) 
        fatal(proc);

      fprintf(stderr, "installboot: premature EOF on %s\n",proc);
      exit(1);
    }

    if (putc(c, imagef) == EOF) 
      fatal(image);
    n--;
  }
  padimage(image, imagef, pad);
}

void make_image(char *image, char **procv)
{
  make_image_aout(image,procv);
  return;
}

void make_image_aout(char *image, char **procv)
/* Collect a set of files in an image, each "segment" is nicely padded out
 * to SECTOR_SIZE, so it may be read from disk into memory without trickery.
 */
{
  NUCS_TRACE(DO_TRACE,"\n");
  FILE *imagef, *procf = 0;
  char *proc, *file;
  int procn;
  struct MNX(image_header_aout) ihdr;
  struct MNX(exec) phdr;
  struct stat st;

  making_image= 1;

  if ((imagef= fopen(image, "w")) == 0) fatal(image);

  for (procn= 0; (proc= *procv++) != 0; procn++) {
    /* Remove the label from the file name. */
    if ((file= strchr(proc, ':')) != 0) file++; else file= proc;

    /* Real files please, may need to seek. */
    if (stat(file, &st) < 0
      || (errno= EISDIR, !S_ISREG(st.st_mode))
      || (procf= fopen(file, "r")) == 0
    ) fatal(proc);

    /* Read a.out header. */
    read_iheader_aout(1, proc, procf, &ihdr);

    /* Scratch. */
    phdr= ihdr.process;

    /* The symbol table is always stripped off. */
    ihdr.process.a_syms= 0;
    ihdr.process.a_flags &= ~A_NSYM;

    /* Write header padded to fill a sector */
    bwrite(imagef, image, &ihdr, sizeof(ihdr));

    padimage(image, imagef, SECTOR_SIZE - sizeof(ihdr));

    /* A page aligned executable needs the header in text. */
    if (phdr.a_flags & A_PAL) {
      rewind(procf);
      phdr.a_text+= phdr.a_hdrlen;
    }

    /* Copy text and data of proc to image. */
    if (phdr.a_flags & A_SEP) {
      /* Separate I&D: pad text & data separately. */

      copyexec(proc, procf, image, imagef, phdr.a_text);
      copyexec(proc, procf, image, imagef, phdr.a_data);
    } else {
      /* Common I&D: keep text and data together. */

      copyexec(proc, procf, image, imagef,
            phdr.a_text + phdr.a_data);
    }

    /* Done with proc. */
    (void) fclose(procf);
  }
  /* Done with image. */

  if (fclose(imagef) == EOF) fatal(image);

  printf("   ------   ------   ------   -------\n");
  printf(" %8ld %8ld %8ld %9ld  total\n",
    total_text, total_data, total_bss,
    total_text + total_data + total_bss);
}

void extractexec(FILE *imagef, char *image, FILE *procf, char *proc,
            long count, off_t *alen)
/* Copy a segment of an executable.  It is padded to a sector in image. */
{
  NUCS_TRACE(DO_TRACE,"\n");
  char buf[SECTOR_SIZE];

  while (count > 0) {
    bread(imagef, image, buf, sizeof(buf));
    *alen-= sizeof(buf);

    bwrite(procf, proc, buf,
      count < sizeof(buf) ? (size_t) count : sizeof(buf));
    count-= sizeof(buf);
  }
}

void extract_image(char *image)
{
  extract_image_aout(image);
  return;
}

void extract_image_aout(char *image)
/* Extract the executables from an image. */
{
  NUCS_TRACE(DO_TRACE,"\n");
  FILE *imagef, *procf;
  off_t len;
  struct stat st;
  struct MNX(image_header_aout) ihdr;
  struct MNX(exec) phdr;
  char buf[SECTOR_SIZE];

  if (stat(image, &st) < 0) 
    fatal(image);

  /* Size of the image. */
  len = S_ISREG(st.st_mode) ? st.st_size : -1;

  if ((imagef = fopen(image, "r")) == 0) fatal(image);

  while (len != 0) {
    /* Extract a program, first sector is an extended header. */
    bread(imagef, image, buf, sizeof(buf));
    len-= sizeof(buf);

    memcpy(&ihdr, buf, sizeof(ihdr));
    phdr= ihdr.process;

    /* Check header. */
    read_iheader_aout(1, ihdr.name, 0, &ihdr);

    if ((procf= fopen(ihdr.name, "w")) == 0)
      fatal(ihdr.name);

    if (phdr.a_flags & A_PAL) {
      /* A page aligned process contains a header in text. */
      phdr.a_text+= phdr.a_hdrlen;
    } else {
      bwrite(procf, ihdr.name, &ihdr.process, phdr.a_hdrlen);
    }

    /* Extract text and data segments. */
    if (phdr.a_flags & A_SEP) {
      extractexec(imagef, image, procf, ihdr.name, phdr.a_text, &len);
      extractexec(imagef, image, procf, ihdr.name, phdr.a_data, &len);
    } else {
      extractexec(imagef, image, procf, ihdr.name, phdr.a_text + phdr.a_data, &len);
    }

    if (fclose(procf) == EOF) fatal(ihdr.name);
  }
}

int rawfd;    /* File descriptor to open device. */
char *rawdev; /* Name of device. */

/* Read blocks on device */
void readblock(off_t blk, char *buf, int block_size)
/* For rawfs, so that it can read blocks. */
{
  NUCS_TRACE(DO_TRACE,"\n");
  int n = 0;

  if (lseek(rawfd, blk * block_size, SEEK_SET) < 0 ||
      (n = read(rawfd, buf, block_size)) < 0) {
    fatal(rawdev);
  }

  if (n < block_size) {
    fprintf(stderr, "installboot: Unexpected EOF on %s\n", rawdev);
    exit(1);
  }
}

void writeblock(MNX(off_t) blk, char *buf, int block_size)
/* Add a function to write blocks for local use. */
{
  NUCS_TRACE(DO_TRACE,"\n");

  if (lseek(rawfd, blk * block_size, SEEK_SET) < 0 ||
      write(rawfd, buf, block_size) < 0) {
    NUCS_TRACE(DO_TRACE,"blk=%d block_size=%d\n",(unsigned int)blk,block_size);
    fatal(rawdev);
  }
}

int raw_install(char *file, off_t *start, off_t *len, int block_size)
/* Copy bootcode or an image to the boot device at the given absolute disk
 * block number.  This "raw" installation is used to place bootcode and
 * image on a disk without a filesystem to make a simple boot disk.  Useful
 * in automated scripts for J. Random User.
 * Note: *len == 0 when an image is read.  It is set right afterwards.
 */
{
  NUCS_TRACE(DO_TRACE,"\n");
  static char buf[_MAX_BLOCK_SIZE];   /* Nonvolatile block buffer. */
  FILE *f;
  MNX(off_t) sec;
  unsigned long devsize;
  static int banner= 0;
  unsigned long long devsize64;

  /* See if the device has a maximum size. */
  devsize= -1;
  if (ioctl(rawfd, BLKGETSIZE64, &devsize64) != 0) {
  fprintf(stderr,
      "installboot: %s: can't device size.\n", __FUNCTION__);
      return 0;
  }
  devsize = (unsigned long)devsize64;

  NUCS_TRACE(DO_TRACE,"devsize=%d\n",(unsigned int)devsize);
  if ((f= fopen(file, "r")) == 0) 
    fatal(file);

  /* Copy sectors from file onto the boot device. */
  sec= *start;
  NUCS_TRACE(DO_TRACE,"sec=%lu len=%lu\n",sec,*len);
  do {
    int off= sec % RATIO(BOOT_BLOCK_SIZE);

    if (fread(buf + off * SECTOR_SIZE, 1, SECTOR_SIZE, f) == 0)
      break;

    if (sec >= devsize) {
      fprintf(stderr, "installboot: %s can't be attached to %s\n", file, rawdev);
      return 0;
    }
    NUCS_TRACE(DO_TRACE,"\n");
    if (off == RATIO(BOOT_BLOCK_SIZE) - 1)
      writeblock(sec / RATIO(BOOT_BLOCK_SIZE), buf, BOOT_BLOCK_SIZE);
  } while (++sec != *start + *len);

  NUCS_TRACE(DO_TRACE,"\n");

  if (ferror(f)) fatal(file);
  (void) fclose(f);

  /* Write a partial block, this may be the last image. */
  if (sec % RATIO(BOOT_BLOCK_SIZE) != 0) 
    writeblock(sec / RATIO(BOOT_BLOCK_SIZE), buf, BOOT_BLOCK_SIZE);

  if (!banner) {
    printf("  sector  length\n");
    banner= 1;
  }
  *len= sec - *start;
  printf("%8ld%8ld  %s\n", *start, *len, file);
  *start= sec;
  return 1;
}

void make_bootable(enum howto how, char* device, char* bootblock,
          char* bootcode, char* path_bootcode, char** imagev)
{

  NUCS_TRACE(DO_TRACE,"\n");
  char buf[_MAX_BLOCK_SIZE + 256], *adrp, *parmp;
  struct fileaddr {
    MNX(off_t) address;
    int count;
  } bootaddr[BOOT_MAX + 1], *bap= bootaddr;

  struct MNX(exec) bootx_exec_aout;
  struct MNX(image_header_aout) iheader_aout;

  struct MNX(exec_elf32) bootx_exec_elf32;
  struct MNX(image_header_elf32) iheader_elf32;
  elf32_ehdr_t ehdr;  // Elf32 header

  struct stat st;           /* for linux */
  struct MNX(stat) MNX(st); /* for minix r_stat function */
  MNX(ino_t) ino;
  MNX(off_t) sector, max_sector;
  FILE *bootf;
  MNX(off_t) addr, fssize, pos, len;
  char *labels, *label, *image;
  int nolabel;
  int block_size = 0;
  enum binfmts binfmt = RAW;


  /* Open device and set variables for readblock. */
  rawdev=device;
  if ((rawfd = open(rawdev, O_RDWR)) < 0) 
    fatal(device);


  /* Read and check the superblock. */
  fssize = r_super(&block_size);
  NUCS_TRACE(DO_TRACE,"ffsize=%lu\n",fssize);

  switch (how) {
    case FS:
      if (fssize == 0) {
        fprintf(stderr,"installboot: %s is not a Minix file system\n",device);
        exit(1);
      }
      break;

    case BOOT:
      if (fssize != 0) {
        int s;
        printf("%s contains a file system!\n", device);
        printf("Scribbling in 10 seconds");
        for (s= 0; s < 10; s++) {
          fputc('.', stdout);
          fflush(stdout);
          sleep(1);
        }
        fputc('\n', stdout);
      }
      fssize= 1;  /* Just a boot block. */
      break;

    default:
      break;
  }

  if (how == FS) {
    /* See if the boot code can be found on the file system. */
    if ((ino = r_lookup(ROOT_INO, bootcode)) == 0) {
      if (errno != ENOENT) fatal(bootcode);
    }
  } else {
    /* Boot code must be attached at the end. */
    ino = 0;
  }

  NUCS_TRACE(DO_TRACE,"ino=%lu\n",ino);

  if (ino == 0) {
  NUCS_TRACE(DO_TRACE,"ino=%lu\n",ino);
    /* For a raw installation, we need to copy the boot code onto
     * the device, so we need to look at the file to be copied.
     */
    if (stat(bootcode, &st) < 0)
      fatal(bootcode);

    if ((bootf= fopen(bootcode, "r")) == 0)
      fatal(bootcode);

  } else {
    /* Boot code is present in the file system. */
    /* Note: minix's stat structure differs from stat structure of linux */
    r_stat(ino, &MNX(st));

    /* Get the header from the first block. */
    if ((addr = r_vir2abs((off_t) 0)) == 0) {
      bootx_exec_aout.a_magic[0] = !A_MAGIC0;
      bootx_exec_elf32.type = !NEXEC;
    } else {
      /* read in the first block of file (on device) */
      readblock(addr, buf, block_size);

      /* Bootcode can be either AOUT or RAW binary format. */
      if (is_aout((char*)buf)) {
        NUCS_TRACE(DO_TRACE,"Type: AOUT\n");

        binfmt = AOUT;
        /* we can copy the header now */
        memcpy(&bootx_exec_aout, buf, sizeof(struct MNX(exec)));

      } else /* RAW */ {
        NUCS_TRACE(DO_TRACE,"Type: RAW\n");

        binfmt = RAW;
      }
    }

    bootf = 0;

    if (binfmt == AOUT)
      iheader_aout.process = bootx_exec_aout;

  }

  /* See if it is an executable (read_iheader does the check). */
  if (binfmt == AOUT) {
    read_iheader_aout(0, bootcode, bootf, &iheader_aout);
    bootx_exec_aout = iheader_aout.process;

  }

  if (bootf != 0)
    fclose(bootf);

  if (binfmt == AOUT) {
    /* Get all the sector addresses of the secondary boot code. */
    max_sector = (bootx_exec_aout.a_hdrlen + bootx_exec_aout.a_text + bootx_exec_aout.a_data +
                  SECTOR_SIZE - 1) / SECTOR_SIZE;

  } else if (binfmt == RAW) {
    /* Get all the sector addresses of the secondary boot code. */
     max_sector = (MNX(st).st_size + SECTOR_SIZE - 1) / SECTOR_SIZE;
  }

  NUCS_TRACE(DO_TRACE,"max_sector=%lu\n",max_sector);

  if (max_sector > BOOT_MAX * RATIO(block_size)) {
    fprintf(stderr, "installboot: %s is way too big\n", bootcode);
    exit(0);
  }

  /* Determine the addresses to the boot code to be patched into the
   * boot block.
   */
  bap->count= 0;  /* Trick to get the address recording going. */

  for (sector= 0; sector < max_sector; sector++) {
    if (ino == 0) {
      addr= fssize + (sector / RATIO(block_size));

    } else if ((addr = r_vir2abs(sector / RATIO(block_size))) == 0) {
      fprintf(stderr, "installboot: %s has holes!\n", bootcode);
      exit(1);
    }

    addr= (addr * RATIO(block_size)) + (sector % RATIO(block_size));

    /* First address of the addresses array? */
    if (bap->count == 0) {
      bap->address= addr;
      NUCS_TRACE(DO_TRACE,"0.Second boot code addr=0x%lx\n",addr);
    }

    /* Paste sectors together in a multisector read. */
    if (bap->address + bap->count == addr)
      bap->count++;
    else {
      /* New address. */
      bap++;
      bap->address= addr;
      bap->count= 1;
      NUCS_TRACE(DO_TRACE,"n. Second boot code addr=0x%lx\n",addr);
    }
  }
  (++bap)->count= 0;  /* No more. */

  /* Examine bootblock now. Input binary can be in aout 
     or in raw format. */

  binfmt = RAW;
  /* Get the boot block (from device) and patch the pieces in. 
     @nucleos: This does probably just an lseek (position set 0).
   */
  readblock(BOOTBLOCK, buf, BOOT_BLOCK_SIZE);

  /* open the bootblock on host */
  if ((bootf = fopen(bootblock, "r")) == 0)
    fatal(bootblock);

  /* examine binfmt of bootblock */
  fread(buf, BOOT_BLOCK_SIZE, 1, bootf);

  if (is_aout((char*)buf)) {
    NUCS_TRACE(DO_TRACE,"Type: aout\n");

    binfmt = AOUT;
    /* we can copy the exec structure now */
    memcpy(&bootx_exec_aout, buf, sizeof(struct MNX(exec)));

  } else /* RAW */{
    NUCS_TRACE(DO_TRACE,"Type: RAW\n");
    binfmt = RAW;
    /* we will exmine the exec structure a bit later */
  }
  /*! examine binfmt of bootblock */

  /* re-open the bootblock on host */
  if ((bootf = freopen(bootblock, "r",bootf)) == 0)
    fatal(bootblock);

  if (binfmt == AOUT) {
    NUCS_TRACE(DO_TRACE,"\n");
    /* examine the rest of exec structure */
    read_iheader_aout(0, bootblock, bootf, &iheader_aout);
    bootx_exec_aout = iheader_aout.process;

    if ((bootx_exec_aout.a_text + bootx_exec_aout.a_data + 4*(bap - bootaddr) + 1) > PARTPOS) {
      fprintf(stderr,"installboot: %s + addresses to %s don't fit in the boot sector\n",
              bootblock, bootcode);
      fprintf(stderr, "You can try copying/reinstalling %s to defragment it\n", bootcode);
      exit(1);
    }
  }

  if (binfmt == AOUT) {
    /* All checks out right.  Read bootblock into the boot block!
       @nucleos: FYI, since we did an fread on this stream (read the header)
                  the position pointer has moved thus the header is not read
                  again. */
    bread(bootf, bootblock, buf, bootx_exec_aout.a_text + bootx_exec_aout.a_data);
    (void) fclose(bootf);
  } else /* RAW */{
    NUCS_TRACE(DO_TRACE,"\n");
    stat(bootblock,&st);
    /* All checks out right.  Read bootblock into the boot block!
       We will path the address at the end of read binary.
       FYI, no read was done before thus reading from the begining. */
    bread(bootf, bootblock, buf, st.st_size);
  }

  /* Patch the addresses in. */
  if (binfmt == AOUT) {
    adrp = buf + (int) (bootx_exec_aout.a_text + bootx_exec_aout.a_data);
  } else /* RAW */ {
    NUCS_TRACE(DO_TRACE,"\n");
    /* In case of raw binary the address will be placed at the end beginig
       with the last double word. */
    adrp = buf + (int) (st.st_size - sizeof(int));
  }

  for (bap= bootaddr; bap->count != 0; bap++) {
    *adrp++= bap->count;
    *adrp++= (bap->address >>  0) & 0xFF;
    *adrp++= (bap->address >>  8) & 0xFF;
    *adrp++= (bap->address >> 16) & 0xFF;
  }

  /* Zero count stops bootblock's reading loop. */
  *adrp++= 0;

  if (bap > bootaddr+1) {
    printf("%s and %d addresses to %s patched into %s\n",
      bootblock, (int)(bap - bootaddr), bootcode, device);
  }

  /* Boot block signature. */
  buf[SIGPOS+0]= (SIGNATURE >> 0) & 0xFF;
  buf[SIGPOS+1]= (SIGNATURE >> 8) & 0xFF;

  /* Sector 2 of the boot block is used for boot parameters, initially
   * filled with null commands (newlines).  Initialize it only if
   * necessary.
   */
  for (parmp= buf + SECTOR_SIZE; parmp < buf + 2*SECTOR_SIZE; parmp++) {
    if (*imagev != 0 || (control(*parmp) && *parmp != '\n')) {
      /* Param sector must be initialized. */
      memset(buf + SECTOR_SIZE, '\n', SECTOR_SIZE);
      break;
    }
  }

  /* Offset to the end of the file system to add boot code and images. */
  pos = fssize * RATIO(block_size);

  if (ino == 0) {
    /* Place the boot code onto the boot device. */
    len = max_sector;
    NUCS_TRACE(DO_TRACE,"len=%ld block_size=%d\n",len,block_size);
    if (!raw_install(bootcode, &pos, &len, block_size)) {
      if (how == FS) {
        fprintf(stderr,"\t(Isn't there a copy of %s on %s that can be used?)\n",
                bootcode, device);
      }
      exit(1);
    }
  }

  parmp= buf + SECTOR_SIZE;
  nolabel= 0;

  if (how == BOOT) {
    /* A boot only disk needs to have floppies swapped. */
    strcpy(parmp,
    "trailer()echo \\nInsert the root diskette then hit RETURN\\n\\w\\c\n");
    parmp+= strlen(parmp);
  }

  while ((imagev != 0) && ((labels= *imagev++) != 0)) {
    /* Place each kernel image on the boot device. */
    NUCS_TRACE(DO_TRACE,"\n");
    if ((image= strchr(labels, ':')) != 0)
      *image++= 0;
    else {
      if (nolabel) {
        fprintf(stderr,
        "installboot: Only one image can be the default\n");
        exit(1);
      }
      nolabel= 1;
      image= labels;
      labels= 0;
    }
    len= 0;

    if (!raw_install(image, &pos, &len, block_size)) 
      exit(1);

    if (labels == 0) {
      /* Let this image be the default. */
      sprintf(parmp, "image=%ld:%ld\n", pos-len, len);
      parmp+= strlen(parmp);
    }

    while (labels != 0) {
      /* Image is prefixed by a comma separated list of
       * labels.  Define functions to select label and image.
       */
      label= labels;
      if ((labels= strchr(labels, ',')) != 0) *labels++ = 0;

      sprintf(parmp,
    "%s(%c){label=%s;image=%ld:%ld;echo %s kernel selected;menu}\n",
        label,
        between('A', label[0], 'Z')
          ? label[0]-'A'+'a' : label[0],
        label, pos-len, len, label);
      parmp+= strlen(parmp);
    }

    if (parmp > buf + block_size) {
      fprintf(stderr,
    "installboot: Out of parameter space, too many images\n");
      exit(1);
    }
  }

  /* Install boot block. */
  writeblock((off_t) BOOTBLOCK, buf, 1024);

  if (pos > fssize * RATIO(block_size)) {
    /* Tell the total size of the data on the device. */
    printf("%16ld  (%ld kb) total\n", pos,
            (pos + RATIO(block_size) - 1) / RATIO(block_size));
  }

  return;
}

void install_master(char *device, char *masterboot, char **guide)
{
  install_master_aout(device, masterboot, guide);
  return;
}

void install_master_aout(char *device, char *masterboot, char **guide)
/* Booting a hard disk is a two stage process:  The master bootstrap in sector
 * 0 loads the bootstrap from sector 0 of the active partition which in turn
 * starts the operating system.  This code installs such a master bootstrap
 * on a hard disk.  If guide[0] is non-null then the master bootstrap is
 * guided into booting a certain device.
 */
{
  NUCS_TRACE(DO_TRACE,"\n");
  FILE *masf;
  unsigned long size;
  struct stat st;
  static char buf[_MAX_BLOCK_SIZE];

  /* Open device. */
  if ((rawfd= open(rawdev= device, O_RDWR)) < 0) fatal(device);

  /* Open the master boot code. */
  if ((masf= fopen(masterboot, "r")) == 0) fatal(masterboot);

  /* See if the user is cloning a device. */
  if (fstat(fileno(masf), &st) >=0 && S_ISBLK(st.st_mode))
    size= PARTPOS;
  else {
    /* Read and check header otherwise. */
    struct MNX(image_header_aout) ihdr;

    read_iheader_aout(1, masterboot, masf, &ihdr);
    size= ihdr.process.a_text + ihdr.process.a_data;
  }
  if (size > PARTPOS) {
    fprintf(stderr, "installboot: %s is too big\n", masterboot);
    exit(1);
  }

  /* Read the master boot block, patch it, write. */
  readblock(BOOTBLOCK, buf, BOOT_BLOCK_SIZE);

  memset(buf, 0, PARTPOS);
  (void) bread(masf, masterboot, buf, size);

  if (guide[0] != 0) {
    /* Fixate partition to boot. */
    char *keys= guide[0];
    char *logical= guide[1];
    size_t i;
    int logfd;
    struct hd_geometry geom;
    unsigned int  offset;
    unsigned long long base;

    /* A string of digits to be seen as keystrokes. */
    i= 0;
    do {
      if (!between('0', keys[i], '9')) {
        fprintf(stderr,
          "installboot: bad guide keys '%s'\n",
          keys);
        exit(1);
      }
    } while (keys[++i] != 0);

    if (size + i + 1 > PARTPOS) {
      fprintf(stderr,
      "installboot: not enough space after '%s' for '%s'\n",
        masterboot, keys);
      exit(1);
    }
    memcpy(buf + size, keys, i);
    size += i;
    buf[size]= '\r';

    if (logical != 0) {
      if ((logfd= open(logical, O_RDONLY)) < 0 || 
         ioctl(logfd,HDIO_GETGEO , &geom) < 0
      ) {
        fatal(logical);
      }

      offset = (unsigned long)(geom.start/SECTOR_SIZE);

      if (size + 5 > PARTPOS) {
        fprintf(stderr,
          "installboot: not enough space "
          "after '%s' for '%s' and an offset "
          "to '%s'\n",
          masterboot, keys, logical);
        exit(1);
      }
      buf[size]= '#';
      memcpy(buf+size+1, &offset, 4);
    }
  }

  /* Install signature. */
  buf[SIGPOS+0]= (SIGNATURE >> 0) & 0xFF;
  buf[SIGPOS+1]= (SIGNATURE >> 8) & 0xFF;

  writeblock(BOOTBLOCK, buf, BOOT_BLOCK_SIZE);
}

void usage(void)
{
  fprintf(stderr,
    "Usage: installboot [-b bootcode] [-B master boot] [-d device] [-i image] [-h] [-l list] [-m] [-t {0,1}] [-x]\n"
    " -b bootcode: path to boot code to be used (on device)\n"
    " -c: create image\n"
    " -B master boot: master boot code to be used\n"
    " -d device: device to be used\n"
    " -i image: image to be used\n"
    " -h: help\n"
    " -l list: list of items separated by `,'\n"
    " -m: install master master boot\n"
    " -p bootcode: path to boot code to be used (on host)\n"
    " -t {0,1}: specifies whether there filesystem installed on device {0==no,1==yes}\n"
    " -x: indicates to extract image specifies by `-i' option\n\n"
    "Examples:\n"
    "Create the image:\n"
    "  installboot -c -i image -l kernel,mm,fs,init\n"
    "Extract image:\n"
    "  installboot -x -i image\n"
    "Make bootable device (with filesystem installed):\n"
    " installboot -d device -B bootblock -b boot [-t 1] [-l list,of,binaries]\n"
    " Note: Filesystem is installed on device is as default.\n"
    "Make bootable device (without filesystem installed):\n"
    "  installboot -d device -B bootblock -b boot -t 0 [-l list,of,binaries]\n"
    " Note: You MUST specify explicitly if there is no filesystem on device.\n"
    "Install master boot:\n"
    "   installboot -m -d device -B masterboot [-l keys,[logical]]\n\n");
  exit(1);
}

int isoption(char *option, char *test)
/* Check if the option argument is equals "test".  Also accept -i as short
 * for -image, and the special case -x for -extract.
 */
{
  NUCS_TRACE(DO_TRACE,"\n");

  if (strcmp(option, test) == 0)
    return 1;

  if (option[0] != '-' && strlen(option) != 2)
    return 0;

  if (option[1] == test[1])
    return 1;

  if (option[1] == 'x' && test[1] == 'e')
    return 1;

  return 0;
}

/* Max. number od tokens in e.g. image vector */
#define MAX_TOKENS 19
/* possible actions for now */
enum actions {CREATE_IMAGE, EXTRACT_IMAGE, INSTALL, INSTALL_MBR};

int main(int argc, char *argv[])
{
  NUCS_TRACE(DO_TRACE,"\n");
  int opt;
  int i = 0;
  enum actions action = INSTALL;

  char* device = 0;
  char* bootblock = 0;
  char* bootcode = 0;
  char* path_bootcode = 0;

  char* image = 0;         /* image to create from the given list */
  char* list[MAX_TOKENS];
  int list_yes = 0;

  enum howto how = FS;     /* assumes filesystem installed */

  int opt_install_master = 0;
  extern char* optarg;
  extern int optind;

  if (argc < 2)
    usage();

  while ((opt = getopt(argc, argv, "b:cB:d:i:hl:mp:t:x")) != -1) {
    switch (opt) {
      case 'b': /* specifies boot code */
        bootcode = optarg;
        break;

      case 'c': /* create image from list of executables */
        action = CREATE_IMAGE;
        break;

      case 'B': /* specifies bootblock (MBR) */
        bootblock = optarg; /* MBR */
        break;

      case 'd': /* specifies device */
        device = optarg;
        break;

      case 'i': /* specifies image */
        /* final image name */
        image = optarg;
        break;

      case 'l': /* gives the list of binaries (separated by `,') */
        /* get the first token */
        list[0] = strtok (optarg," ,");

        /* at least one vector required */
        if(image[0])
          list_yes=1;

        /* parse the rest of string */
        while ((++i <= MAX_TOKENS) && ((list[i] = strtok (0," ,")) != 0));
        break;

      case 'm': /* master */
        action = INSTALL_MBR;
        break;

      case 'p': /* path to boot code inside image */
        path_bootcode=optarg;
        break;

      case 't': /* filesystem installed or not installed on device */
        if(atoi(optarg))
          how=FS;
        else if(!atoi(optarg))
          how=BOOT;
        break;

      case 'x': /* extract */
        action = EXTRACT_IMAGE;
        break;

      case 'h':
        usage();
        exit(0);

      default: /* '?' */
        usage();
        exit(1);
    }
  }

  if(!list_yes)
    memset(list,0,MAX_TOKENS);

  /* Create the image `image' from the given list `list' */
  if (action == CREATE_IMAGE && image && list_yes) {
    NUCS_TRACE(DO_TRACE,"\n");
    /* {image, imagev} */
    make_image(image, list);
    exit(0);
  }

  /* Extract the image `ximage' */
  if (action == EXTRACT_IMAGE && image) {
    NUCS_TRACE(DO_TRACE,"\n");
    extract_image(image);
    exit(0);
  }

 /* Install bootblock on the bootsector of device with the disk addresses to
    bootcode patched into the data segment of bootblock.  "How" tells if there
    should or shoudn't be a file system on the disk.  The images in the list
    vector are added to the end of the device (if exists). */
  if (action == INSTALL && device && how == FS && bootblock && bootcode && path_bootcode) {
    NUCS_TRACE(DO_TRACE,"\n");
    /* {how, device, bootblock, bootcode, imagev} */
    make_bootable(FS, device, bootblock, bootcode, path_bootcode, list);
    exit(0);
  }

  if (action == INSTALL && device && how == BOOT && bootblock && bootcode && path_bootcode) {
    NUCS_TRACE(DO_TRACE,"\n");
    /* {how, device, bootblock, bootcode, imagev} */
    make_bootable(BOOT, device, bootblock, bootcode, path_bootcode, list);
    exit(0);
  }

  /* Booting a hard disk is a two stage process:  The master bootstrap in sector
     0 loads the bootstrap from sector 0 of the active partition which in turn
     starts the operating system.  This code installs such a master bootstrap
     on a hard disk.  If guide[0] is non-null then the master bootstrap is
     guided into booting a certain device. */
  if (action == INSTALL_MBR && device && bootblock) {
    NUCS_TRACE(DO_TRACE,"\n");
    install_master(device, bootblock, list);
    exit(0);
  }

  fprintf(stderr,"Wrong compination of options\n");
  usage();

  exit(1);
}
