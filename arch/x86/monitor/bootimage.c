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
#include <stdlib.h>
#include <stdio.h>
#include <nucleos/stddef.h>
#include <nucleos/types.h>
#include <nucleos/stat.h>
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
#include <asm/bootparam.h>
#include <asm/setup.h>

#include "rawfs.h"
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
#define KERNEL_IDX	0       /* The first process is the kernel. */
#define KERNEL_IMAGE_PATH	"/boot/image"

static int block_size = 0;
static u32_t (*vir2sec)(u32_t vsec);   /* Where is a sector on disk? */
static int serial_line = -1;

/* Per-process memory adresses. */
struct process {
	u32 entry;	/* Entry point. */
	u32 cs;		/* Code segment. */
	u32 ds;		/* Data segment. */
	u32 data;	/* Data start */
	u32 end;	/* End of this process, size = (end - cs). */
};

#define IM_NAME_MAX 63

struct image_header {
	char name[IM_NAME_MAX + 1];	/* Null terminated. */
	struct exec process;
};

static struct process procs[MAX_IMG_PROCS_COUNT];

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
			raw_copy(initrd_base + (j*SECTOR_SIZE - initrd_hdr.a_hdrlen),
				 mon2abs(get_sector(j)), SECTOR_SIZE);
		}

		/* We will get from extented memroy */
		raw_copy(mon2abs(&super_v3_magic), initrd_base + SUPER_V3_MAGIC_OFFSET, 2);

		if (super_v3_magic != SUPER_V3_MAGIC) {
			printf("error: Can't find MINIX3 super magic (%s)\n", initrd);
			return -1;
		}

		printf("done\n");
	} else {
		printf("error: can't load initrd\n");
		return -1;
	}

	return 0;
}

/* select/check given  initrd path*/
static unsigned long select_kimage(char* kimage)
{
	ino_t kimage_ino;
	struct stat st;
	unsigned long size;

	fsok = (r_super(&block_size) != 0);
	kimage_ino = r_lookup(ROOT_INO, kimage);

	if (!fsok || !kimage_ino) {
		printf("error: kernel image %s: %s\n", kimage, unix_err(errno));
		return 0;
	}

	r_stat(kimage_ino, &st);

	vir2sec = file_vir2sec;
	if (vir2sec < 0) {
		printf("Bad block size or unsuspected EOF\n");
		return 0;
	}

	size = (st.st_size + SECTOR_SIZE - 1) >> SECTOR_SHIFT;

	return (size * SECTOR_SIZE);
}

static int load_kimage(char* kimage, u32 load_addr, u32 kimage_size, u32 limit)
{
	int nvsec = kimage_size/SECTOR_SIZE;
	int i = 0;

	printf("Loading kernel image '%s' at 0x%x (size %dKiB)...", kimage, load_addr, kimage_size/1024);

	/* Clear the area where initrd will be placed. */
	raw_clear(load_addr, kimage_size);

	for (i = 0; i < nvsec; i++) {
		char *buf = get_sector(i);

		if (!buf)
			return -1;

		raw_copy(load_addr + i*SECTOR_SIZE, mon2abs(buf), SECTOR_SIZE);
	}

	printf("done\n");

	return 0;
}

static u32 unpack_kimage_inplace(u32 kimage_addr, u32 kimage_size, u32 limit,
				 u8 *aout_hdrs_buf, struct process *procs)
{
	struct image_header hdr;
	u32 kernel_space_end = 0;
	u32 addr;
	struct process *procp;
	u32 text_data_size, text_size, bss_size, stack_size;
	u32 bss_addr;
	long processor = boot_params.nucleos_kludge.processor;
	int verbose = 1;
	int proc_count = 0;

	/* The stack is pretty deep here, so check if heap and stack collide. */
	sbrk(0);

	addr = kimage_addr;
	procp = procs;

	/* Read the many different processes: */
	while (kimage_size) {
		u32_t start_addr;
		start_addr = addr;

		if (++proc_count == MAX_IMG_PROCS_COUNT) {
			printf("There are more then %d programs in image\n", MAX_IMG_PROCS_COUNT);
			return 0;
		}

		/* Read header. */
		raw_copy(mon2abs(&hdr), addr, sizeof(hdr));

		if (BADMAG(hdr.process)) {
			printf("Image contains a bad program header\n");
			return 0;
		}

		/* Sanity check: an 8086 can't run a 386 kernel. */
		if (hdr.process.a_cpu == A_I80386 && processor < 386) {
			printf("You can't run a 386 kernel on this 80%ld\n", processor);
			return 0;
		}
		/* Save a copy of the header for the kernel, with a_syms
		 * misused as the address where the process is loaded at.
		 */
		hdr.process.a_syms = addr;

		memcpy(aout_hdrs_buf + (proc_count - 1)*A_MINHDR, &hdr.process, A_MINHDR);

		/* get rid of header of size SECTOR_SIZE */
		raw_copy(start_addr, start_addr + SECTOR_SIZE, kimage_size - SECTOR_SIZE);
		kimage_size -= SECTOR_SIZE;

		if (((verbose) ? (verbose++) : 0) == 1)
			printf("       cs         ds     text     data      bss    stack\n");

		/* Supposed (minimal) segment sizes. */
		text_size = hdr.process.a_text;
		/* Add text to data to form one segment. */
		text_data_size = hdr.process.a_text + hdr.process.a_data;
		bss_size = hdr.process.a_bss;
		stack_size = hdr.process.a_total - text_data_size - bss_size;

		/* needed just for accessing kernel magic number in data segment */
		procp->data = addr + text_size;

		/* Collect info about the process to be. */
		procp->cs = addr;
		procp->ds = procp->cs;
		procp->entry = hdr.process.a_entry;

		text_data_size = align(text_data_size, PAGE_SIZE);

		/* bss start address */
		addr += text_data_size;
		bss_addr = addr;
		bss_size = align(bss_size, PAGE_SIZE);

		if (addr + bss_size > limit) {
			printf("Not enough memory to unpack the image\n");
			return 0;
		}

		/* stack start address, align to page size  */
		addr += bss_size;
		stack_size = align(stack_size, PAGE_SIZE);

		if (verbose) {
			printf("0x%07lx  0x%07lx %8ld %8ld %8ld",
				procp->cs, procp->ds, text_size,
				(text_data_size - text_size), bss_size);
		}

		if (verbose)
			printf(" %8ld", stack_size);

		/* end of stack */
		addr += stack_size;

		/* Process endpoint. */
		procp->end = addr;
		kernel_space_end = addr;

		if (verbose)
			printf("  %s\n", hdr.name);

		/* copy the rest of the code and data at end of current one */
		u32 next_kimage_addr = start_addr + text_data_size;

		kimage_size -= text_data_size;

		if (next_kimage_addr + kimage_size > addr) {
			if (next_kimage_addr + kimage_size > limit) {
				printf("Not enough memory to unpacki the image!\n");
				return 0;
			}

			raw_copy(next_kimage_addr + kimage_size, next_kimage_addr, kimage_size);
			raw_copy(addr, next_kimage_addr + kimage_size, kimage_size);
		} else
			raw_copy(addr, next_kimage_addr, kimage_size);

		/* Zero out bss. */
		raw_clear(bss_addr, bss_size);

		/* next process image */
		procp++;
	}

	/* Return the end of kernel space */
	return kernel_space_end;
}

/* Avoid multi-spaces here */
char *cmdline = "rootdev=/dev/c0d0p0 ramimagedev=/dev/c0d0p0";
static char cmd_line_params[COMMAND_LINE_SIZE];
static int cmdline_len = 0;

static int exec_image(struct process *procs)
{
	u16 kdata_magic_num = 0;

	/* Check the kernel magic number (located in data section). */
	raw_copy(mon2abs(&kdata_magic_num), procs[KERNEL_IDX].data + MAGIC_OFF, 2);
	if (kdata_magic_num != KERNEL_D_MAGIC) {
		printf("Kernel magic number is incorrect (0x%x)\n", kdata_magic_num);
		return -1;
	}

	/* Set the video to the required mode. */
	if (boot_params.nucleos_kludge.vdu_vga)
		set_mode(COLOR_MODE);
	else
		set_mode(MONO_MODE);

	minix(procs[KERNEL_IDX].entry, procs[KERNEL_IDX].cs, procs[KERNEL_IDX].ds, mon2abs(&boot_params));

	return -1;
}

static u8 aout_hdrs_buf[MAX_IMG_PROCS_COUNT*A_MINHDR];

int boot_nucleos(void)
{
	char *kimage_name;
	u32 kimage_addr;
	u32 kimage_size;
	u32 limit;
	u32 kernel_end;

	kimage_name = KERNEL_IMAGE_PATH;
	if ((kimage_size = select_kimage(kimage_name)) == 0)
		return -1;

	/* Load the image into this memory block. This should be
	 * above 1M and the code should run in protected mode.
	 */
	kimage_addr = mem[1].base;
	kimage_addr = align(kimage_addr, PAGE_SIZE);

	limit = mem[1].base + mem[1].size;

	if (serial_line >= 0) {
		char linename[2];
		linename[0] = serial_line + '0';
		linename[1] = '\0';
		b_setvar(E_VAR, SERVARNAME, linename);
	}

	/* Clear the area where the headers will be placed. */
	memset(aout_hdrs_buf, 0, MAX_IMG_PROCS_COUNT*A_MINHDR);

	if (load_kimage(kimage_name, kimage_addr, kimage_size, limit) < 0) {
		printf("Can't load kernel image %s at 0x%x, (size: %dKiB)\n",
		       kimage_name, kimage_addr, kimage_size/1024);
		return -1;
	}

	kernel_end = unpack_kimage_inplace(kimage_addr, kimage_size, limit, aout_hdrs_buf, procs);
	if (!kernel_end) {
		printf("Couldn't unpack the kernel!\n");
		return -1;
	}

	/* Check whether we are loading kernel with memory which has builtin initrd. */
	u32 ramdisk_image = kernel_end;

	/* The select_initrd() returns number of sectors. Skip the sector with a header. */
	u32 ramdisk_size = (select_initrd(INITRD) - 1) * SECTOR_SIZE;

	if (ramdisk_size) {
		if (load_initrd(INITRD, ramdisk_image) < 0)
			return -1;

		/* fill the header */
		raw_copy(kimage_addr + offsetof(struct setup_header, ramdisk_image) + 0x1f1,
			 mon2abs(&ramdisk_image), sizeof(ramdisk_image));

		raw_copy(kimage_addr + offsetof(struct setup_header, ramdisk_size) + 0x1f1,
			 mon2abs(&ramdisk_size), sizeof(ramdisk_size));
	} else {
		printf("Ramdisk not found.\n");
	}

	/* Close the disk. */
	dev_close();

	/* Setup command-line for kernel */
	memset(cmd_line_params, 0, COMMAND_LINE_SIZE);

	/* Add static command-line string. The 2 trailing zeros indicates the
	 * end of command line.
	 */
	if (strlen(cmd_line_params) > COMMAND_LINE_SIZE - 2)
		return -1;

	memcpy(cmd_line_params, cmdline, strlen(cmdline));
	cmdline_len = strlen(cmd_line_params);

	/* add aout headers address */
	boot_params.nucleos_kludge.aout_hdrs_addr = mon2abs(aout_hdrs_buf);

	/* Translate the command-line for kernel. DON'T ADD ANY NEW OPTIONS */
	int j = 0;
	for (j = 0; j < cmdline_len; j++)
		if (cmd_line_params[j] == ' ')
			cmd_line_params[j] = 0;

	/* fill the header */
	u32 cmd_line_params_addr = (u32)mon2abs(cmd_line_params);
	raw_copy(kimage_addr + offsetof(struct setup_header, cmd_line_ptr) + 0x1f1,
		 mon2abs(&cmd_line_params_addr), sizeof(&cmd_line_params_addr));

	if (exec_image(procs) < 0) {
		printf("Can't execute image %s\n", kimage_name);
		return -1;
	}

	return 0;
}
