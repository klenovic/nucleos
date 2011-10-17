/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*      bootimage.c - Load an image and start it.       Author: Kees J. Bot
 *                    19 Jan 1992
 */
#include <nucleos/stddef.h>
#include <nucleos/types.h>
#include <nucleos/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <nucleos/limits.h>
#include <nucleos/string.h>
#include <nucleos/errno.h>
#include <nucleos/a.out.h>
#include <nucleos/const.h>
#include <nucleos/type.h>
#include <nucleos/syslib.h>
#include <nucleos/tty.h>
#include <nucleos/video.h>
#include <kernel/const.h>
#include <kernel/types.h>
#include <ibm/partition.h>
#include <asm/page_types.h>
#include "rawfs.h"
#include "image.h"
#include "boot.h"

/* Align a to a multiple of n (a power of 2): */
#define align(a, n)	(((u32_t)(a) + ((u32_t)(n) - 1)) & (~((u32_t)(n) - 1)))
/* Check VFS_PROC_NR magic */
#define SUPER_V3_MAGIC         0x4d5a
#define SUPER_V3_MAGIC_OFFSET  0x418
/* Magic numbers in process' data space. */
#define MAGIC_OFF	0       /* Offset of magic # in data seg. */
#define FLAGS_EXT_OFF	2       /* Offset in kernel text to extended flags. */
#define KERNEL_D_MAGIC	0x526F  /* Kernel magic number. */

static int block_size = 0;
static unsigned short k_flags_ext;   /* Extended flags. */
static u32_t (*vir2sec)(u32_t vsec);   /* Where is a sector on disk? */
static int serial_line = -1;

#ifndef CONFIG_BUILTIN_INITRD
/* Path specified by configuration */
#define INITRD CONFIG_INITRD_TARGET_PATH
#else
/* just define some value (default) */
#define INITRD "/boot/initrd"
#endif

/* Load the initial ramdisk */
#define K_BUILTIN_INITRD	0x0001 /* Kernel has builtin initrd (inside memory driver). */

/* Data about the different processes. */
#define PROCESS_MAX	16      /* Must match the space in kernel/mpx.x */
#define KERNEL_IDX	0       /* The first process is the kernel. */

struct process {  /* Per-process memory adresses. */
	u32_t entry;    /* Entry point. */
	u32_t cs;       /* Code segment. */
	u32_t ds;       /* Data segment. */
	u32_t data;     /* To access the data segment. */
	u32_t end;      /* End of this process, size = (end - cs). */
};

static struct process procs[PROCESS_MAX];
static int n_procs;        /* Number of processes. */

static void raw_clear(u32_t addr, u32_t count)
/* Clear "count" bytes at absolute address "addr". */
{
	static char zeros[128];
	u32_t dst;
	u32_t zct;

	zct= sizeof(zeros);

	if (zct > count) zct= count;

	raw_copy(addr, mon2abs(&zeros), zct);
	count-= zct;

	while (count > 0) {
		dst= addr + zct;

		if (zct > count) zct= count;

		raw_copy(dst, addr, zct);
		count-= zct;
		zct*= 2;
	}
}

/* Repackage the environment settings for the kernel. */
static int params2params(char *params, size_t psize)
{
	size_t i, n;
	environment *e;
	char *name, *value;
	dev_t dev;

	i= 0;
	for (e= env; e != 0; e= e->next) {
		name= e->name;
		value= e->value;

		if (!(e->flags & E_VAR)) continue;

		if (e->flags & E_DEV) {
			if ((dev= name2dev(value)) == -1) return 0;
			value= ul2a10((u16_t) dev);
		}

		n= i + strlen(name) + 1 + strlen(value) + 1;

		if (n < psize) {
			strcpy(params + i, name);
			strcat(params + i, "=");
			strcat(params + i, value);
		}

		i= n;
	}

	if (i >= psize) {
		printf("Too many boot parameters\n");
		return 0;
	}

	params[i]= 0;   /* End marked with empty string. */
	return 1;
}

/* Return the size of a process in sectors as found in an image. */
static u32_t proc_size(struct image_header *hdr)
{
	u32_t len= hdr->process.a_text;

	if (hdr->process.a_flags & A_PAL)
		len+= hdr->process.a_hdrlen;

	len= align(len + hdr->process.a_data, SECTOR_SIZE);

	return len >> SECTOR_SHIFT;
}

/* Translate a virtual sector number to an absolute disk sector. */
static u32_t file_vir2sec(u32_t vsec)
{
	off_t blk;

	if(!block_size)
		return -1;

	if ((blk = r_vir2abs(vsec / RATIO(block_size))) == -1)
		return -1;

	return blk == 0 ? 0 : lowsec + blk * RATIO(block_size) + vsec % RATIO(block_size);
}

static char *get_sector(u32_t vsec)
/* Read a sector "vsec" from the image into memory and return its address.
 * Return null on error.
 */
{
	u32_t sec;
	int r;

	static char buf[SECTOR_SIZE];
	static size_t count;      /* Number of sectors in the buffer. */
	static u32_t bufsec;      /* First Sector now in the buffer. */

	/* First sector; initialize. */
	if (vsec == 0)
		count = 0;

	if ((sec = (*vir2sec)(vsec)) == -1)
		return 0;

	if (sec == 0) {
		/* A hole. */
		count = 0;
		memset(buf, 0, SECTOR_SIZE);
		return buf;
	}

	/* Can we return a sector from the buffer? */
	if ((sec - bufsec) < count)
		return buf + ((size_t) (sec - bufsec) << SECTOR_SHIFT);

	/* Not in the buffer. */
	count = 1;
	bufsec = sec;

	do {
		if (dev_boundary(bufsec + count))
			break;

		if ((sec = (*vir2sec)(++vsec)) == -1)
			break;

		/* Consecutive? */
		if (sec != bufsec + count)
			break;
	} while (0);

	/* Actually read the sectors. */
	if ((r = readsectors(mon2abs(buf), bufsec, count)) != 0) {
		readerr(bufsec, r);
		count = 0;
		return 0;
	}

	return buf;
}

static int get_segment(u32_t *vsec, long *size, u32_t *addr, u32_t limit)
/* Read *size bytes starting at virtual sector *vsec to memory at *addr. */
{
	char* buf;
	size_t n;

	while (*size > 0) {
		if ((buf = get_sector((*vsec)++)) == 0)
			return 0;

		if (*addr + PAGE_SIZE > limit)
			return 0;

		raw_copy(*addr, mon2abs(buf), SECTOR_SIZE);
		*addr += SECTOR_SIZE;
		*size -= SECTOR_SIZE;
		buf += SECTOR_SIZE;
	}

	/* Zero extend to a page size */
	n = align(*addr, PAGE_SIZE) - *addr;
	raw_clear(*addr, n);
	*addr += n;
	*size -= n;

	return 1;
}

/* select/check given  initrd path*/
static unsigned long select_initrd(char* initrd)
{
	ino_t initrd_ino;
	struct stat st;
	unsigned long size;

	fsok = (r_super(&block_size) != 0);
	initrd_ino = r_lookup(ROOT_INO, initrd);

	if (!fsok || !initrd_ino) {
		printf("error: initrd %s: %s\n", initrd, unix_err(errno));
		return 0;
	}

	r_stat(initrd_ino, &st);

	vir2sec = file_vir2sec;
	if (vir2sec < 0) {
		printf("Bad block size or unsuspected EOF\n");
		return 0;
	}

	size = (st.st_size + SECTOR_SIZE - 1) >> SECTOR_SHIFT;

	return size;
}

/* Load the initial ramdisk at absolute address */
static int load_initrd(char* initrd, unsigned long load_addr)
{
	struct exec initrd_hdr;
	unsigned long initrd_base = 0;
	long initrd_size = 0;
	int nvsec = 0;
	unsigned short super_v3_magic = 0;
	int j=0;
	char* buf = 0;

	printf("Loading initrd %s at 0x%x ...", initrd, load_addr);

	if ((nvsec = select_initrd(initrd)) > 0) {
		buf = get_sector(0);

		if (!buf) {
			printf("error: can't get sector (%s)\n", initrd);
			return -1;
		}

		/* For header check */
		memcpy(&initrd_hdr, buf, sizeof(initrd_hdr));

		/* Finish if not aout or not an image */
		if (BADMAG(initrd_hdr) || !(initrd_hdr.a_flags & A_IMG)) {
			printf("error: bad header (%s)\n", initrd);
			return -1;
		}

		initrd_base = load_addr;
		initrd_size = initrd_hdr.a_data;

		/* Clear the area where initrd will be placed. */
		raw_clear(initrd_base, initrd_size);

		/* Copy first sector but skip the header */
		raw_copy(initrd_base, mon2abs(get_sector(0) + initrd_hdr.a_hdrlen), SECTOR_SIZE - initrd_hdr.a_hdrlen);

		for (j=1; j<nvsec; j++) {
			raw_copy(initrd_base + (j*SECTOR_SIZE - initrd_hdr.a_hdrlen), mon2abs(get_sector(j)), SECTOR_SIZE);
		}

		/* We will get from extented memroy */
		raw_copy(mon2abs(&super_v3_magic), initrd_base + SUPER_V3_MAGIC_OFFSET, 2);

		if (super_v3_magic != SUPER_V3_MAGIC) {
			printf("error: Can't find MINIX3 super magic (%s)\n", initrd);
			return -1;
		}

		/* Let to know to kernel */
		b_setvar(E_SPECIAL|E_VAR, "initrdbase", ul2a10(initrd_base));
		b_setvar(E_SPECIAL|E_VAR, "initrdsize", ul2a10(initrd_size));

		printf("done\n");
	} else {
		printf("error: can't load initrd\n");
		return -1;
	}

	return 0;
}

/* Get a Nucleos image into core */
static struct process *load_image(const u32 image_addr, u32 image_size, const u32 aout_hdrs_addr,
				  const u32 limit, struct process *procs)
{
	int i;
	struct image_header hdr;
	char *buf;
	u32_t vsec, n, totalmem = 0;
	u32 addr;
	struct process *procp;
	long a_text, a_data, a_bss, a_stack;
	long processor = a2l(b_value("processor"));
	int verbose = 1;

	/* The stack is pretty deep here, so check if heap and stack collide. */
	sbrk(0);

	vsec = 0;		/* Load this sector from image next. */
	addr = image_addr;
	procp = procs;

	/* Read the many different processes: */
	for (i = 0; vsec < image_size; i++) {
		u32_t startaddr;
		startaddr = addr;

		if (i == PROCESS_MAX) {
			printf("There are more then %d programs in image\n", PROCESS_MAX);
			return 0;
		}

		/* Read header. */
		for (;;) {
			if ((buf = get_sector(vsec++)) == 0)
				return 0;

			memcpy(&hdr, buf, sizeof(hdr));

			if (BADMAG(hdr.process)) {
				printf("Image contains a bad program header\n");
				return 0;
			} else
				break;

			/* Bad label, skip this process. */
			vsec += proc_size(&hdr);
		}

		/* Sanity check: an 8086 can't run a 386 kernel. */
		if (hdr.process.a_cpu == A_I80386 && processor < 386) {
			printf("You can't run a 386 kernel on this 80%ld\n", processor);
			return 0;
		}

		/* Get the click shift from the kernel text segment. */
		if (i == KERNEL_IDX) {
			char *textp;

			if ((textp = get_sector(vsec)) == 0)
				return 0;

			k_flags_ext = *((u16_t*)(textp + FLAGS_EXT_OFF));
		}

		/* Save a copy of the header for the kernel, with a_syms
		 * misused as the address where the process is loaded at.
		 */
		hdr.process.a_syms = addr;

		raw_copy(aout_hdrs_addr + i * A_MINHDR, mon2abs(&hdr.process), A_MINHDR);

		if (((verbose) ? (verbose++) : 0) == 1)
			printf("       cs         ds     text     data      bss    stack\n");

		/* Segment sizes. */
		a_text = hdr.process.a_text;
		a_data = hdr.process.a_data;
		a_bss = hdr.process.a_bss;

		a_stack = hdr.process.a_total - a_data - a_bss - a_text;

		/* Collect info about the process to be. */
		procp->cs = addr;

		/* Process may be page aligned so that the text segment contains
		 * the header, or have an unmapped zero page against vaxisms.
		 */
		procp->entry = hdr.process.a_entry;

		if (hdr.process.a_flags & A_PAL)
			a_text += hdr.process.a_hdrlen;

		if (hdr.process.a_flags & A_UZP)
			procp->cs -= PAGE_SIZE;

		/* Add text to data to form one segment. */
		procp->data = addr + a_text;
		procp->ds = procp->cs;
		a_data += a_text;

		/* Read the data segment. */
		if (!get_segment(&vsec, &a_data, &addr, limit))
			return 0;

		if (verbose) {
			printf("0x%07lx  0x%07lx %8ld %8ld %8ld",
				procp->cs, procp->ds,
				hdr.process.a_text, hdr.process.a_data,
				hdr.process.a_bss);
		}

		if (verbose)
			printf(" %8ld", a_stack);

		/* Note that a_data may be negative now, but we can look at it
		 * as -a_data bss bytes.
		 */

		/* Compute the number of bss clicks left. */
		a_bss += a_data;
		n = align(a_bss, PAGE_SIZE);
		a_bss -= n;

		/* Zero out bss. */
		if (addr + n > limit) {
			printf("Not enough memory to load image\n");
			return 0;
		}

		raw_clear(addr, n);
		addr += n;

		/* And the number of stack clicks. */
		a_stack += a_bss;
		n = align(a_stack, PAGE_SIZE);
		a_stack -= n;

		/* Add space for the stack. */
		addr += n;

		/* Process endpoint. */
		procp->end = addr;

		if(verbose)
			printf("  %s\n", hdr.name);
		else {
			u32_t mem;
			mem = addr - startaddr;
			printf("%s ", hdr.name);
			totalmem += mem;
		}

		/* next process image */
		procp++;
	}

	if(!verbose)
		printf("(%dk)\n", totalmem/1024);

	if ((n_procs = i) == 0) {
		printf("There are no programs in image\n");
		return 0;
	}

	/* Check whether we are loading kernel with memory which has builtin initrd. */
	printf("Initial ramdisk...");
	if (!(k_flags_ext & K_BUILTIN_INITRD)) {
		printf("not builtin.\n");
		if (load_initrd(INITRD, addr) < 0)
			return 0;
	} else {
		printf("builtin.\n");
	}

	/* Close the disk. */
	dev_close();

	return procs;
}

static int exec_image(struct process *procs, u32 aout_hdrs_addr)
{
	u16_t mode;
	char *console;
	char params[SECTOR_SIZE];
	u16 kdata_magic_num = 0;

	/* Check the kernel magic number (located in data section). */
	raw_copy(mon2abs(&kdata_magic_num), procs[KERNEL_IDX].data + MAGIC_OFF, 2);
	if (kdata_magic_num != KERNEL_D_MAGIC) {
		printf("Kernel magic number is incorrect (0x%x)\n", kdata_magic_num);
		return -1;
	}

	/* Translate the boot parameters for kernel. */
	if (!params2params(params, sizeof(params))) {
		printf("Can't translate boot parameters!");
		return -1;
	}

	/* Set the video to the required mode. */
	if ((console = b_value("console")) == 0 || (mode= a2x(console)) == 0)
		mode = strcmp(b_value("chrome"), "color") == 0 ? COLOR_MODE : MONO_MODE;

	set_mode(mode);

	minix(procs[KERNEL_IDX].entry, procs[KERNEL_IDX].cs, procs[KERNEL_IDX].ds, params,
	      sizeof(params), aout_hdrs_addr);

	return -1;
}

/**
 * Recursively read the current directory
 * Note: One can't use r_stat while reading a directory.
 */
static ino_t latest_version(char *version, struct stat *stp)
{
	char name[NAME_MAX + 1];
	ino_t ino, newest;

	if ((ino= r_readdir(name)) == 0)
		return 0;

	newest = latest_version(version, stp);
	r_stat(ino, stp);

	if (S_ISREG(stp->st_mode)) {
		newest= ino;
		strcpy(version, name);
	}

	return newest;
}

/* Look image up on the filesystem, if it is a file then we're done.
 * Setup the found image size. */
static char *select_image(char *image, u32 *image_size)
{
	ino_t image_ino;
	struct stat st;

	image = strcpy(malloc((strlen(image) + 1 + NAME_MAX + 1)*sizeof(char)), image);

	fsok = r_super(&block_size) != 0;

	if (!fsok || (image_ino = r_lookup(ROOT_INO, image)) == 0) {
		if (!fsok)
			printf("No image selected\n");
		else
			printf("Can't load %s: %s\n", image, unix_err(errno));
		return 0;
	}

	r_stat(image_ino, &st);

	if (!S_ISREG(st.st_mode)) {
		char *version= image + strlen(image);
		char dots[NAME_MAX + 1];

		if (!S_ISDIR(st.st_mode)) {
			printf("%s: %s\n", image, unix_err(ENOTDIR));
			return 0;
		}

		r_readdir(dots);
		r_readdir(dots); /* "." & ".." */
		*version++= '/';
		*version= 0;

		if ((image_ino = latest_version(version, &st)) == 0) {
			printf("There are no images in %s\n", image);

			return 0;;
		}

		r_stat(image_ino, &st);
	}

	vir2sec = file_vir2sec;
	if (vir2sec < 0) {
		printf("Bad block size or unsuspected EOF\n");
		return 0;
	}

	*image_size = ((st.st_size + SECTOR_SIZE - 1) >> SECTOR_SHIFT);

	return image;
}

int boot_nucleos(void)
{
	char *image_name;
	u32 image_addr;
	u32 image_size;
	u32 aout_hdrs_addr;
	u32 limit;

	if ((image_name = select_image(b_value("image"), &image_size)) == 0)
		return -1;

	/* Load the image into this memory block. This should be
	 * above 1M and the code should run in protected mode.
	 */
	image_addr = mem[1].base;
	image_addr = align(image_addr, PAGE_SIZE);

	limit = mem[1].base + mem[1].size;

	/* Allocate and clear the area where the headers will be placed.
	 * The headers are placed below 1M at (1M - PROCESS_MAX * A_MINHDR).
	 * Note that we will need some additional space here for real-mode kernel.
	 */
	aout_hdrs_addr = mem[0].base + mem[0].size - PROCESS_MAX * A_MINHDR;

	/* Clear the area where the headers will be placed. */
	raw_clear(aout_hdrs_addr, PROCESS_MAX * A_MINHDR);

	if (serial_line >= 0) {
		char linename[2];
		linename[0] = serial_line + '0';
		linename[1] = '\0';
		b_setvar(E_VAR, SERVARNAME, linename);
	}

	printf("\nLoading image '%s'\n", image_name);
	if (!load_image(image_addr, image_size, aout_hdrs_addr, limit, procs)) {
		printf("Can't load image %s at 0x%x, (size: %dB aout headers addr: 0x%x)\n",
		        image_addr, image_size, aout_hdrs_addr);
		return -1;
	}

	if (exec_image(procs, aout_hdrs_addr) < 0) {
		printf("Can't execute image %s\n", image_name);
		return -1;
	}

	return 0;
}
