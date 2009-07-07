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
 * @note This is an zero ended array and planning to
 *       be a linked list in the future.
 */
extern struct nucleos_binfmt *__binfmts[];

/**
 * @brief Number of registered binary formats
 * @note The zero at the end of list is not considered.
 */
extern int num_binfmts;

/**
 * @brief Get list of known binary format handlers
 * @return pointer to known format handlers
 */
static inline struct nucleos_binfmt **get_binfmts(void)
{
	return __binfmts;
}

/**
 * @brief Get binary format handler
 * @param id  binary format ID
 * @retur pointer to handler on success otherwise 0
 */
static inline struct nucleos_binfmt *get_binfmt_handler(enum e_binfmt id)
{
	struct nucleos_binfmt **bf = 0;

	/* Beware of gaps in the list binary formats otherwise it may
	 * happens that no handler will found.
	 */
	for (bf = __binfmts; *bf != 0; bf++) {
		if ((*bf)->id == id)
			break;
	}

	return *bf;
}

/**
 * @brief Find format handler for executable file
 * @param param[out]  binary format parameters
 * @param vp  pointer to vnode
 * @param binfmts  list of known formats
 * @retur format ID on succes otherwise -1
 */
static inline int find_binfmt_handler(struct nucleos_binprm *param, struct vnode *vp,
				      struct nucleos_binfmt **binfmts)
{
	int ret;
	struct nucleos_binfmt **bf = 0;

	/* does not expect gaps in the list of structure */
	for (bf = binfmts; *bf != 0; bf++) {
		if ((*bf)->check_binfmt) {
			/* this could return format id or ESCRIPT too */
			ret = (*bf)->check_binfmt(param, vp);

			if (ret == (*bf)->id || ret == ESCRIPT)
				return ret;
		}
	}

	/* no handler for this format */
	return -1;
}

/* @klenovic: Once it's linked list then implement these methods */
/**
 * @brief Register new binary format handler
 * @param binfmt  pointer to new binary format handler
 * @return 0 on success
 */
static inline int register_binfmt(struct nucleos_binfmt *binfmt)
{
	return 0;
}

/**
 * @brief Unregister binary format handler
 * @param binfmt  pointer to registered binary format handler
 * @note This function will never fails even if not registered handler
 *       is trying to be unregistered.
 */
static inline void unregister_binfmt(struct nucleos_binfmt *binfmt)
{
	return;
}

#endif /* __NUCLEOS_BINFMTS_H */
