/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __NUCLEOS_TYPE_H
#define __NUCLEOS_TYPE_H

#include <nucleos/types.h>
#include <kernel/const.h>
#include <nucleos/const.h>

/* Type definitions. */
typedef unsigned int vir_clicks;	/* virtual addr/length in clicks */
typedef unsigned long phys_bytes;	/* physical addr/length in bytes */
typedef unsigned int phys_clicks;	/* physical addr/length in clicks */

#ifdef CONFIG_X86_32
typedef long unsigned int vir_bytes;	/* virtual addresses/lengths in bytes */
#endif

/* Memory map for local text, stack, data segments. */
struct mem_map {
	vir_clicks mem_vir;		/* virtual address */
	phys_clicks mem_phys;		/* physical address */
	vir_clicks mem_len;		/* length */
};

/* Memory map for remote memory areas, e.g., for the RAM disk. */
struct far_mem {
	int in_use;			/* entry in use, unless zero */
	phys_clicks mem_phys;		/* physical address */
	vir_clicks mem_len;		/* length */
};

/* Structure for virtual copying by means of a vector with requests. */
struct vir_addr {
	int proc_nr_e;
	int segment;
	vir_bytes offset;
};

#define phys_cp_req vir_cp_req 
struct vir_cp_req {
	struct vir_addr src;
	struct vir_addr dst;
	phys_bytes count;
};

typedef struct {
	vir_bytes iov_addr;		/* address of an I/O buffer */
	vir_bytes iov_size;		/* sizeof an I/O buffer */
} iovec_t;

typedef struct {
	int iov_grant;		/* grant ID of an I/O buffer */
	vir_bytes iov_size;		/* sizeof an I/O buffer */
} iovec_s_t;

/* PM passes the address of a structure of this type to KERNEL when
 * sys_sigsend() is invoked as part of the signal catching mechanism.
 * The structure contain all the information that KERNEL needs to build
 * the signal stack.
 */
struct sigmsg {
	int sm_signo;			/* signal number being caught */
	unsigned long sm_mask;	/* mask to restore when handler returns */
	vir_bytes sm_sighandler;	/* address of handler */
	vir_bytes sm_sigreturn;	/* address of sigreturn in C library */
	vir_bytes sm_stkptr;		/* user stack pointer */
};

/* This is used to obtain system information through SYS_GETINFO. */
struct kinfo {
	phys_bytes code_base;		/* base of kernel code */
	phys_bytes code_size;		
	phys_bytes data_base;		/* base of kernel data */
	phys_bytes data_size;
	vir_bytes proc_addr;		/* virtual address of process table */
	phys_bytes _kmem_base;	/* kernel memory layout (/dev/kmem) */
	phys_bytes _kmem_size;
	phys_bytes bootdev_base;	/* boot device from boot image (/dev/boot) */
	phys_bytes bootdev_size;
	phys_bytes ramdev_base;	/* boot device from boot image (/dev/boot) */
	phys_bytes ramdev_size;
	phys_bytes _params_base;	/* parameters passed by boot monitor */
	phys_bytes _params_size;
	int nr_procs;			/* number of user processes */
	int nr_tasks;			/* number of kernel tasks */
	char release[6];		/* kernel release number */
	char version[6];		/* kernel version number */
};

/* Load data accounted every this no. of seconds. */
#define _LOAD_UNIT_SECS		 6 	/* Changing this breaks ABI. */

/* Load data history is kept for this long. */
#define _LOAD_HISTORY_MINUTES	15	/* Changing this breaks ABI. */
#define _LOAD_HISTORY_SECONDS	(60*_LOAD_HISTORY_MINUTES)

/* We need this many slots to store the load history. */
#define _LOAD_HISTORY	(_LOAD_HISTORY_SECONDS/_LOAD_UNIT_SECS)

/* Runnable processes and other load-average information. */
struct loadinfo {
	u16_t proc_load_history[_LOAD_HISTORY];	/* history of proc_s_cur */
	u16_t proc_last_slot;
	clock_t last_clock;
};

struct machine {
	int pc_at;
	int ps_mca;
	int processor;
	int padding;	/* used to be protected */
	int vdu_ega;
	int vdu_vga;
};

struct io_range
{
	unsigned ior_base;	/* Lowest I/O port in range */
	unsigned ior_limit;	/* Highest I/O port in range */
};

struct mem_range
{
	phys_bytes mr_base;	/* Lowest memory address in range */
	phys_bytes mr_limit;	/* Highest memory address in range */
};

/* For EXEC_NEWMEM */
struct exec_newmem
{
	vir_bytes text_bytes;
	vir_bytes data_bytes;
	vir_bytes bss_bytes;
	vir_bytes tot_bytes;
	vir_bytes args_bytes;
	vir_bytes entry_point;
	int sep_id;
	dev_t st_dev;
	ino_t st_ino;
	time_t st_ctime;
	uid_t new_uid;
	gid_t new_gid;
	char progname[PROC_NAME_LEN];	/* Should be at least PROC_NAME_LEN */
};

/* Memory chunks. */
struct memory {
	phys_bytes	base;
	phys_bytes	size;
};

#define STATICINIT(v, n) \
	if(!(v)) {	\
		if(!((v) = alloc_contig(sizeof(*(v)) * (n), 0, NULL))) { \
			panic(__FILE__, "allocating " #v " failed", n);	\
		}	\
	}

/* The kernel outputs diagnostic messages in a circular buffer. */
struct kmessages {
	int km_next;                          /* next index to write */
	int km_size;                          /* current size in buffer */
	char km_buf[KMESS_BUF_SIZE];          /* buffer for messages */
};

#include <asm/irq_vectors.h>

/* randomness struct: random sources after interrupts: */
#define RANDOM_SOURCES			NR_IRQ_VECTORS
#define RANDOM_ELEMENTS			64

typedef unsigned short rand_t;

struct k_randomness {
	int random_elements, random_sources;
	struct k_randomness_bin {
		int r_next;                             /* next index to write */
		int r_size;                             /* number of random elements */
		rand_t r_buf[RANDOM_ELEMENTS]; /* buffer for random info */
	} bin[RANDOM_SOURCES];
};

#endif /* __NUCLEOS_TYPE_H */
