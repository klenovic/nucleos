/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/**
 * @file include/nucleos/binfmts.h
 * @brief handling of various binary formats
 */ 
#ifndef __NUCLEOS_BINFMTS_H
#define __NUCLEOS_BINFMTS_H

#include <string.h>
#include <stdint.h>
#include <errno.h>

#define BINFMT_BUFSIZE	128

/** @brief Number of known binary formats */
#define NUM_BINFMTS	2

/* Returned by read_header for a #! script. */
#define ESCRIPT		(-2000)

struct vnode;
struct exec_newmem;

/** @brief Known binary formats */
enum e_binfmt {
	BINFMT_ELF = 0x01,	/* ELF32 or ELF64 */
	BINFMT_AOUT = 0x02,	/* legacy minix3 aout format */
};

extern struct nucleos_binfmt binfmt_aout;
extern struct nucleos_binfmt binfmt_elf32;

struct nucleos_binprm {
	unsigned char buf[BINFMT_BUFSIZE];
	struct vnode *vp;
	struct exec_newmem ex;
	int proc_e;
	int load_text;
	vir_bytes stack_top;
	int allow_setuid;
	unsigned char checked;
};

struct nucleos_binfmt {
	/* id of bin. format */
	enum e_binfmt id;
	/* call to check the binary */
	int (*check_binfmt)(struct nucleos_binprm *param, struct vnode *vp);
	/* handle loading */
	int (*load_binary)(struct nucleos_binprm *param);
};

/**
 * @brief Array of known binary formats
 * @note This is planning to be a linked list.
 */
extern struct nucleos_binfmt *__binfmts[NUM_BINFMTS];

static inline struct nucleos_binfmt **get_binfmts(void)
{
	return __binfmts;
}

static inline struct nucleos_binfmt *get_binfmt_handler(enum e_binfmt id)
{
	int i;

	for(i=0; i<NUM_BINFMTS; i++) {
		if (__binfmts[i]->id == id)
			break;
	}

	if (i == NUM_BINFMTS)
		return 0;

	return __binfmts[i];
}

static inline int find_binfmt_handler(struct nucleos_binprm *param, struct vnode *vp,
				      struct nucleos_binfmt **binfmts)
{
	int i;
	int ret;

	for(i=0; i<NUM_BINFMTS; i++) {
		if (binfmts[i]->check_binfmt) {
			/* this could return format id or ESCRIPT too */
			ret = binfmts[i]->check_binfmt(param, vp);

			if (ret == binfmts[i]->id || ret == ESCRIPT)
				return ret;
		}
	}

	/* no handler for this format */
	return -1;
}


static inline int register_binfmt(struct nucleos_binfmt *binfmt)
{
	int i;

	/* find a free slot for binary format */
	for (i=0; i<NUM_BINFMTS; i++) {
		if (!__binfmts[i]) {
			__binfmts[i] = binfmt;
			break;
		}
	}

	if (i == NUM_BINFMTS)
		return -1;

	return 0;
}

static inline int register_binfmts(void)
{
	int i;
	int count_bfmts = 0;
	struct nucleos_binfmt **bf = get_binfmts();

	for (i=0; i<NUM_BINFMTS; i++, bf++) {
		if (bf[i]) {
			if (!register_binfmt(bf[i]))
				count_bfmts++;
		}
	}
	
	/* any registered formats? */
	if (!count_bfmts)
		return -1;
	
	return 0;
}

static inline void unregister_binfmt(struct nucleos_binfmt *binfmt) 
{
	int i;

	/* find a the binary format handler */
	for (i=0; i<NUM_BINFMTS; i++) {
		if (!__binfmts[i]) {
			if (__binfmts[i]->id == binfmt->id) {
				__binfmts[i] = 0;
				break;
			}
		}
	}
}

#endif /* __NUCLEOS_BINFMTS_H */
