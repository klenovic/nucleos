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

static int block_size = 0;

extern int serial_line;
extern u16_t vid_port;         /* Video i/o port. */
extern u32_t vid_mem_base;     /* Video memory base address. */
extern u32_t vid_mem_size;     /* Video memory size. */

#ifndef CONFIG_BUILTIN_INITRD
/* Path specified by configuration */
#define INITRD CONFIG_INITRD_TARGET_PATH
#else
/* just define some value (default) */
#define INITRD "/boot/initrd"
#endif

/* select/check given  initrd path*/
static unsigned long select_initrd(char* initrd);

/* Load the initial ramdisk at absolute address */
int load_initrd(char* initrd, unsigned long loadaddr);

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
} process[PROCESS_MAX];
int n_procs;        /* Number of processes. */

/* Magic numbers in process' data space. */
#define MAGIC_OFF	0       /* Offset of magic # in data seg. */
#define FLAGS_EXT_OFF	2       /* Offset in kernel text to extended flags. */
#define KERNEL_D_MAGIC	0x526F  /* Kernel magic number. */

void raw_clear(u32_t addr, u32_t count)
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

/* Align a to a multiple of n (a power of 2): */
#define align(a, n)	(((u32_t)(a) + ((u32_t)(n) - 1)) & (~((u32_t)(n) - 1)))
unsigned short k_flags_ext;   /* Extended flags. */
u32_t reboot_code;            /* Obsolete reboot code return pointer. */

int params2params(char *params, size_t psize)
/* Repackage the environment settings for the kernel. */
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

u32_t proc_size(struct image_header *hdr)
/* Return the size of a process in sectors as found in an image. */
{
	u32_t len= hdr->process.a_text;

	if (hdr->process.a_flags & A_PAL)
		len+= hdr->process.a_hdrlen;

	len= align(len + hdr->process.a_data, SECTOR_SIZE);

	return len >> SECTOR_SHIFT;
}

off_t image_off, image_size;
u32_t (*vir2sec)(u32_t vsec);   /* Where is a sector on disk? */

u32_t file_vir2sec(u32_t vsec)
/* Translate a virtual sector number to an absolute disk sector. */
{
	off_t blk;

	if(!block_size) {
		errno = 0;
		return -1;
	}

	if ((blk= r_vir2abs(vsec / RATIO(block_size))) == -1) {
		errno= EIO;
		return -1;
	}

	return blk == 0 ? 0 : lowsec + blk * RATIO(block_size) + vsec % RATIO(block_size);
}

u32_t flat_vir2sec(u32_t vsec)
/* Simply add an absolute sector offset to vsec. */
{
	return lowsec + image_off + vsec;
}

char *get_sector(u32_t vsec)
/* Read a sector "vsec" from the image into memory and return its address.
 * Return 0 on error.  (This routine tries to read an entire track, so
 * the next request is usually satisfied from the track buffer.)
 */
{
	u32_t sec;
	int r;
#define SECBUFS 1
	static char buf[SECBUFS * SECTOR_SIZE];
	static size_t count;      /* Number of sectors in the buffer. */
	static u32_t bufsec;      /* First Sector now in the buffer. */

	if (vsec == 0) count= 0;  /* First sector; initialize. */

	if ((sec= (*vir2sec)(vsec)) == -1) return 0;

	if (sec == 0) {
		/* A hole. */
		count= 0;
		memset(buf, 0, SECTOR_SIZE);
		return buf;
	}

	/* Can we return a sector from the buffer? */
	if ((sec - bufsec) < count) {
		return buf + ((size_t) (sec - bufsec) << SECTOR_SHIFT);
	}

	/* Not in the buffer. */
	count= 0;
	bufsec= sec;

	/* Read a whole track if possible. */
	while (++count < SECBUFS && !dev_boundary(bufsec + count)) {
		vsec++;
		if ((sec= (*vir2sec)(vsec)) == -1) break;

		/* Consecutive? */
		if (sec != bufsec + count) break;
	}

	/* Actually read the sectors. */
	if ((r= readsectors(mon2abs(buf), bufsec, count)) != 0) {
		readerr(bufsec, r);
		count= 0;
		errno= 0;
		return 0;
	}

	return buf;
}

int get_clickshift(u32_t ksec, struct image_header *hdr)
/* Get the click shift and special flags from kernel text. */
{
	char *textp;

	if ((textp = get_sector(ksec)) == 0)
		return 0;

	if (hdr->process.a_flags & A_PAL)
		textp += hdr->process.a_hdrlen;

	k_flags_ext = *((u16_t*)(textp + FLAGS_EXT_OFF));

	return 1;
}

int get_segment(u32_t *vsec, long *size, u32_t *addr, u32_t limit)
/* Read *size bytes starting at virtual sector *vsec to memory at *addr. */
{
	char* buf=0;
	size_t cnt, n;

	cnt= 0;
	while (*size > 0) {
		if (cnt == 0) {
			if ((buf= get_sector((*vsec)++)) == 0)
				return 0;

			cnt= SECTOR_SIZE;
		}

		if (*addr + PAGE_SIZE > limit) {
			errno = ENOMEM;
			return 0;
		}

		n = PAGE_SIZE;

		if (n > cnt) n= cnt;

		raw_copy(*addr, mon2abs(buf), n);
		*addr+= n;
		*size-= n;
		buf+= n;
		cnt-= n;
	}

	/* Zero extend to a click. */
	n= align(*addr, PAGE_SIZE) - *addr;
	raw_clear(*addr, n);
	*addr+= n;
	*size-= n;

	return 1;
}

/* Load image from disk into memory `packed_image_addr' */
int load_image(char *packed_image_addr)
{
	return 0;
}

/* Extract packet image from address packed_image_addr to address extract_image_addr */
int extract_image(char *extract_image_addr, char *packed_image_addr)
{
	return 0;
}

void exec_image(char *image)
/* Get a Nucleos image into core, patch it up and execute. */
{
	int i;
	struct image_header hdr;
	char *buf;
	u32_t vsec, addr, limit, n, totalmem = 0;
	struct process *procp;    /* Process under construction. */
	long a_text, a_data, a_bss, a_stack;
	long processor= a2l(b_value("processor"));
	u16_t mode;
	char *console;
	char params[SECTOR_SIZE];
	char *verb;
	u16 kdata_magic_num = 0;
	u32 aout_hdrs_addr;
	int verbose = 1;

	/* The stack is pretty deep here, so check if heap and stack collide. */
	sbrk(0);

	if ((verb= b_value("verbose")) != 0 && a2l(verb) > 0)
		verbose = 1;

	printf("\nLoading image '%s'\n", image);

	vsec = 0;           /* Load this sector from image next. */

	addr = mem[1].base; /* Load the image into this memory block. This should be
			     * above 1M and the code should run in protected mode.
			     */
	limit = mem[1].base + mem[1].size;

	/* Allocate and clear the area where the headers will be placed.
	 * The headers are placed below 1M at (1M - PROCESS_MAX * A_MINHDR).
	 * Note that we will need some additional space here for real-mode kernel.
	 */
	aout_hdrs_addr = mem[0].base + mem[0].size - PROCESS_MAX * A_MINHDR;

	/* Clear the area where the headers will be placed. */
	raw_clear(aout_hdrs_addr, PROCESS_MAX * A_MINHDR);

	/* Read the many different processes: */
	for (i= 0; vsec < image_size; i++) {
		u32_t startaddr;
		startaddr = addr;
		if (i == PROCESS_MAX) {
			printf("There are more then %d programs in %s\n", PROCESS_MAX, image);
			errno= 0;
			return;
		}

		procp= &process[i];

		/* Read header. */
		for (;;) {
			if ((buf= get_sector(vsec++)) == 0) return;

			memcpy(&hdr, buf, sizeof(hdr));

			if (BADMAG(hdr.process)) {
				errno = ENOEXEC;
				return;
			} else
				break;

			/* Bad label, skip this process. */
			vsec += proc_size(&hdr);
		}

		/* Sanity check: an 8086 can't run a 386 kernel. */
		if (hdr.process.a_cpu == A_I80386 && processor < 386) {
			printf("You can't run a 386 kernel on this 80%ld\n", processor);
			errno= 0;
			return;
		}

		/* Get the click shift from the kernel text segment. */
		if (i == KERNEL_IDX) {
			if (!get_clickshift(vsec, &hdr))
				return;

			addr = align(addr, PAGE_SIZE);
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
		procp->entry= hdr.process.a_entry;

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
			return;

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
			errno = ENOMEM;
			return;
		}

		raw_clear(addr, n);
		addr += n;

		/* And the number of stack clicks. */
		a_stack += a_bss;
		n = align(a_stack, PAGE_SIZE);
		a_stack -= n;

		/* Add space for the stack. */
		addr+= n;

		/* Process endpoint. */
		procp->end= addr;

		if(verbose)
			printf("  %s\n", hdr.name);
		else {
			u32_t mem;
			mem = addr-startaddr;
			printf("%s ", hdr.name);
			totalmem += mem;
		}
	}

	if(!verbose)
		printf("(%dk)\n", totalmem/1024);

	if ((n_procs= i) == 0) {
		printf("There are no programs in %s\n", image);
		errno= 0;
		return;
	}

	/* Check the kernel magic number (located in data section). */
	raw_copy(mon2abs(&kdata_magic_num), process[KERNEL_IDX].data + MAGIC_OFF, 2);
	if (kdata_magic_num != KERNEL_D_MAGIC) {
		printf("Kernel magic number is incorrect (0x%x)\n", kdata_magic_num);
		errno= 0;
		return;
	}

	/* Check whether we are loading kernel with memory which has builtin initrd. */
	printf("Initial ramdisk...");
	if (!(k_flags_ext & K_BUILTIN_INITRD)) {
		printf("not builtin.\n");
		if (load_initrd(INITRD, addr) < 0)
			return;
	} else {
		printf("builtin.\n");
	}

	/* Translate the boot parameters to what Minix likes best. */
	if (!params2params(params, sizeof(params))) {
		errno = 0;
		return;
	}

	/* Set the video to the required mode. */
	if ((console= b_value("console")) == 0 || (mode= a2x(console)) == 0) {
		mode= strcmp(b_value("chrome"), "color") == 0 ? COLOR_MODE : MONO_MODE;
	}

	set_mode(mode);

	/* Close the disk. */
	dev_close();

	minix(process[KERNEL_IDX].entry, process[KERNEL_IDX].cs, process[KERNEL_IDX].ds, params,
	      sizeof(params), aout_hdrs_addr);

	printf("Error while booting kernel!\n");

	return;
}

/**
 * Recursively read the current directory
 * Note: One can't use r_stat while reading a directory.
 */
ino_t latest_version(char *version, struct stat *stp)
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

char *select_image(char *image)
/* Look image up on the filesystem, if it is a file then we're done, but
 * if its a directory then we want the newest file in that directory.  If
 * it doesn't exist at all, then see if it is 'number:number' and get the
 * image from that absolute offset off the disk.
 */
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
		goto bail_out;
	}

	r_stat(image_ino, &st);

	if (!S_ISREG(st.st_mode)) {
		char *version= image + strlen(image);
		char dots[NAME_MAX + 1];

		if (!S_ISDIR(st.st_mode)) {
			printf("%s: %s\n", image, unix_err(ENOTDIR));
			goto bail_out;
		}

		r_readdir(dots);
		r_readdir(dots); /* "." & ".." */
		*version++= '/';
		*version= 0;

		if ((image_ino = latest_version(version, &st)) == 0) {
			printf("There are no images in %s\n", image);

			goto bail_out;
		}

		r_stat(image_ino, &st);
	}

	vir2sec = file_vir2sec;
	image_size = (st.st_size + SECTOR_SIZE - 1) >> SECTOR_SHIFT;

	return image;

bail_out:
	free(image);
	return 0;
}

/* Check VFS_PROC_NR magic */
#define SUPER_V3_MAGIC         0x4d5a
#define SUPER_V3_MAGIC_OFFSET  0x418

/* Load the initial ramdisk at absolute address */
int load_initrd(char* initrd, unsigned long load_addr)
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
	size = (st.st_size + SECTOR_SIZE - 1) >> SECTOR_SHIFT;

	return size;
}

void boot_nucleos(void)
{
	char *image;

	if ((image = select_image(b_value("image"))) == 0)
		return;

	if (serial_line >= 0) {
		char linename[2];
		linename[0] = serial_line + '0';
		linename[1] = '\0';
		b_setvar(E_VAR, SERVARNAME, linename);
	}

	exec_image(image);

	switch (errno) {
	case ENOEXEC:
		printf("%s contains a bad program header\n", image);
		break;
	case ENOMEM:
		printf("Not enough memory to load %s\n", image);
		break;
	case EIO:
		 printf("Unsuspected EOF on %s\n", image);
	case 0:
	  /* No error or error already reported. */;
	}

	free(image);
}
