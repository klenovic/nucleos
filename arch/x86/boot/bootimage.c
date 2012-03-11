/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/types.h>
#include <nucleos/string.h>
#include <nucleos/a.out.h>
#include <asm/page_types.h>
#include <asm/bootparam.h>
#include <asm/setup.h>
#include "boot.h"

/* Align a to a multiple of n (a power of 2): */
#define align(a, n)	(((u32_t)(a) + ((u32_t)(n) - 1)) & (~((u32_t)(n) - 1)))

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

	addr = kimage_addr;
	procp = procs;

	/* Read the many different processes: */
	while (kimage_size > 1024) {
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

static char cmd_line_params[COMMAND_LINE_SIZE];

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

	pm_nucleos(procs[KERNEL_IDX].entry, procs[KERNEL_IDX].cs, procs[KERNEL_IDX].ds, mon2abs(&boot_params));

	return -1;
}

static u8 aout_hdrs_buf[MAX_IMG_PROCS_COUNT*A_MINHDR];

int boot_nucleos(void)
{
	u32 kimage_addr;
	u32 kimage_size;
	u32 limit;
	u32 kernel_end;

	/* Load the image into this memory block. This should be
	 * above 1M and the code should run in protected mode.
	 */
	kimage_addr = mem[1].base;
	kimage_addr = align(kimage_addr, PAGE_SIZE);

	limit = mem[1].base + mem[1].size;

	/* Clear the area where the headers will be placed. */
	memset(aout_hdrs_buf, 0, MAX_IMG_PROCS_COUNT*A_MINHDR);

	/* hdr.syssize filled by build.c */
	kimage_size = hdr.syssize;
	kimage_size = kimage_size * 16 - 4;

	kernel_end = unpack_kimage_inplace(kimage_addr, kimage_size, limit, aout_hdrs_buf, procs);
	if (!kernel_end) {
		printf("Couldn't unpack the kernel!\n");
		return -1;
	}

	/* Setup command-line for kernel */
	memset(cmd_line_params, 0, COMMAND_LINE_SIZE);

	/* add aout headers address */
	boot_params.nucleos_kludge.aout_hdrs_addr = mon2abs(aout_hdrs_buf);

	/* save header into boot parameters */
	memcpy(&boot_params.hdr, &hdr, sizeof(hdr));

	/* copy the command-line on safer place */
	raw_copy(mon2abs(cmd_line_params), boot_params.hdr.cmd_line_ptr, COMMAND_LINE_SIZE);

	if (exec_image(procs) < 0) {
		printf("Can't execute image\n");
		return -1;
	}

	return 0;
}
