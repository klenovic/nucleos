/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include "fs.h"
#include <sys/stat.h>
#include <nucleos/callnr.h>
#include <nucleos/endpoint.h>
#include <nucleos/com.h>
#include <nucleos/u64.h>
#include <nucleos/elf.h>
#include <nucleos/binfmts.h>
#include <nucleos/vfsif.h>
#include <nucleos/a.out.h>
#include <signal.h>
#include <string.h>
#include <dirent.h>

#include "fproc.h"
#include "param.h"
#include "vnode.h"
#include "vmnt.h"

static int patch_stack(struct vnode *vp, char stack[ARG_MAX], vir_bytes *stk_bytes);
static int insert_arg(char stack[ARG_MAX], vir_bytes *stk_bytes, char *arg, int replace);
static void patch_ptr(char stack[ARG_MAX], vir_bytes base);
static void clo_exec(struct fproc *rfp);

#define PTRSIZE		sizeof(char *)	/* Size of pointers in argv[] and envp[]. */

/**
 * @brief Perform the execve(name, argv, envp) call
 * @param proc_e  process number (endpoint)
 * @param path  executable path
 * @param path_len  executable path length (including terminating null)
 * @param frame  frame pointer (arguments and environments)
 * @param frame_len  size of frame
 * @return OK on success
 */
int pm_exec(int proc_e, char *path, vir_bytes path_len, char *frame, vir_bytes frame_len)
{
/* Perform the execve(name, argv, envp) call.  The user library builds a
 * complete stack image, including pointers, args, environ, etc.  The stack
 * is copied to a buffer inside FS, and then to the new core image.
 */
	int r, round, proc_s;
	vir_bytes vsp;
	struct fproc *rfp;
	struct vnode *vp;
	char *cp;
	struct stat sb;
	static char mbuf[ARG_MAX];	/* buffer for stack and zeroes */
	struct nucleos_binprm bfmt_param;
	struct nucleos_binfmt *bhandler;

	okendpt(proc_e, &proc_s);

	rfp = fp = &fproc[proc_s];
	who_e = proc_e;
	who_p = proc_s;
	super_user = (fp->fp_effuid == SU_UID ? TRUE : FALSE);   /* su? */

	bfmt_param.proc_e = proc_e;

	/* Get the exec file name. */
	r = fetch_name(path, path_len, 0);

	if (r != OK) {
		printf("pm_exec: fetch_name failed\n");
		printf("return at %s, %d\n", __FILE__, __LINE__);

		return(r);	/* file name not in user data segment */
	}

	/* Fetch the stack from the user before destroying the old core image. */
	if (frame_len > ARG_MAX) {
		printf("pm_exec: bad frame_len\n");
		printf("return at %s, %d\n", __FILE__, __LINE__);

		return(ENOMEM);	/* stack too big */
	}

	bfmt_param.ex.args_bytes = frame_len;
	r = sys_datacopy(proc_e, (vir_bytes) frame, SELF, (vir_bytes) mbuf,
			(phys_bytes)bfmt_param.ex.args_bytes);

	/* can't fetch stack (e.g. bad virtual addr) */
	if (r != OK) {
		printf("pm_exec: sys_datacopy failed\n");
		printf("return at %s, %d\n", __FILE__, __LINE__);

		return(r);
	}

	/* The default is the keep the original user and group IDs */
	bfmt_param.ex.new_uid = rfp->fp_effuid;
	bfmt_param.ex.new_gid = rfp->fp_effgid;

	/* round = 0 (first attempt), or 1 (interpreted script) */
	for (round = 0; round < 2; round++) {
#if 0
		printf("vfs:pm_exec: round %d, name '%s'\n", round, user_fullpath);
#endif
		/* Save the name of the program */
		(cp = strrchr(user_fullpath, '/')) ? cp++ : (cp= user_fullpath);

		strncpy(bfmt_param.ex.progname, cp, PROC_NAME_LEN-1);
		bfmt_param.ex.progname[PROC_NAME_LEN-1] = '\0';

		/* Request lookup */
		if ((r = lookup_vp(0 /*flags*/, 0 /*!use_realuid*/, &vp)) != OK)
			return r;

		if ((vp->v_mode & I_TYPE) != I_REGULAR) {
			put_vnode(vp);
			return ENOEXEC;
		}

		/* Check access. */
		if ((r = forbidden(vp, X_BIT, 0 /*!use_realuid*/)) != OK) {
			put_vnode(vp);
			return r;
		}

		/* Get ctime */
		r = req_stat(vp->v_fs_e, vp->v_inode_nr, FS_PROC_NR, (char *)&sb, 0);

		if (r != OK) {
			put_vnode(vp);
			return r;
		}

		bfmt_param.ex.st_ctime = sb.st_ctime;
#if 0
		printf("vfs:pm_exec: round %d, mode 0%o, uid %d, gid %d\n",
		round, vp->v_mode, vp->v_uid, vp->v_gid);
#endif
		if (round == 0) {
			/* Deal with setuid/setgid executables */
			if (vp->v_mode & I_SET_UID_BIT)
				bfmt_param.ex.new_uid = vp->v_uid;

			if (vp->v_mode & I_SET_GID_BIT)
				bfmt_param.ex.new_gid = vp->v_gid;
		}

#ifdef CONFIG_DEBUG_VFS_BINFMT
		app_dbg("Looking for format handler ...\n");
#endif
		/* go throught all registered binary formats and choose one which conforms */
		r = find_binfmt_handler(&bfmt_param, vp, get_binfmts());
#ifdef CONFIG_DEBUG_VFS_BINFMT
		app_dbg("Found format handler ID=0x%x\n", r);
#endif
		if (r != ESCRIPT || round != 0) {
			/* get the format handler according to found id */
			bhandler = get_binfmt_handler(r);
#ifdef CONFIG_DEBUG_VFS_BINFMT
			app_dbg("handler ID=0%x\n",bhandler->id);
#endif
			r = OK;
			break;
		}
#ifdef CONFIG_DEBUG_VFS_BINFMT
		app_dbg("Continue processing...\n");
#endif
		/* Get fresh copy of the file name. */
		r = fetch_name(path, path_len, 0);

		if (r != OK) {
			printf("pm_exec: 2nd fetch_name failed\n");
			put_vnode(vp);
			return(r); /* strange */
		}

		bfmt_param.ex.args_bytes = frame_len;
		r = patch_stack(vp, mbuf, &bfmt_param.ex.args_bytes);

		put_vnode(vp);

		if (r != OK) {
			printf("pm_exec: patch stack\n");
			printf("return at %s, %d\n", __FILE__, __LINE__);
			return r;
		}
	}

	if (r != OK) {
		printf("pm_exec: returning ENOEXEC, r = %d\n", r);
		printf("pm_exec: progname = '%s'\n", bfmt_param.ex.progname);
		put_vnode(vp);
		return ENOEXEC;
	}

	r = bhandler->load_binary(&bfmt_param);

	if (r != OK) {
		app_err("Can't load binary\n");
		put_vnode(vp);

		return r;
	}

	/* Patch up stack and copy it from FS to new core image. */
	vsp = bfmt_param.stack_top;
	vsp -= bfmt_param.ex.args_bytes;

	patch_ptr(mbuf, vsp);
	r = sys_datacopy(SELF, (vir_bytes) mbuf, proc_e, (vir_bytes) vsp,
			(phys_bytes)bfmt_param.ex.args_bytes);

	if (r != OK) {
		printf("vfs: datacopy returns %d trying to copy to %p\n", r, vsp);
		panic(__FILE__,"pm_exec stack copy err on", proc_e);
	}

	put_vnode(vp);

	if (r != OK) {
		printf("return at %s, %d\n", __FILE__, __LINE__);
		return r;
	}

	clo_exec(rfp);

	if (bfmt_param.allow_setuid) {
		rfp->fp_effuid = bfmt_param.ex.new_uid;
		rfp->fp_effgid = bfmt_param.ex.new_gid;
	}

	/* This child has now exec()ced. */
	rfp->fp_execced = 1;

	/* Check if this is a driver that can now be useful. */
	dmap_endpt_up(rfp->fp_endpoint);

	return OK;
}

/*===========================================================================*
 *				patch_stack				     *
 *===========================================================================*/
static int patch_stack(vp, stack, stk_bytes)
struct vnode *vp;		/* pointer for open script file */
char stack[ARG_MAX];		/* pointer to stack image within FS */
vir_bytes *stk_bytes;		/* size of initial stack */
{
/* Patch the argument vector to include the path name of the script to be
 * interpreted, and all strings on the #! line.  Returns the path name of
 * the interpreter.
 */
	enum { INSERT=FALSE, REPLACE=TRUE };
	int n, r;
	off_t pos;
	char *sp, *interp = NULL;
	u64_t new_pos;
	unsigned int cum_io_incr;
	char buf[_MAX_BLOCK_SIZE];

	/* Make user_fullpath the new argv[0]. */
	if (!insert_arg(stack, stk_bytes, user_fullpath, REPLACE)) return(ENOMEM);

	pos = 0;	/* Read from the start of the file */

	/* Issue request */
	r = req_readwrite(vp->v_fs_e, vp->v_inode_nr, vp->v_index, cvul64(pos),
			  READING, FS_PROC_NR, buf, _MAX_BLOCK_SIZE, &new_pos, &cum_io_incr);

	if (r != OK) return r;
	
	n = vp->v_size;

	if (n > _MAX_BLOCK_SIZE)
		n = _MAX_BLOCK_SIZE;

	if (n < 2) return ENOEXEC;
	
	sp = &(buf[2]);				/* just behind the #! */
	n -= 2;

	if (n > PATH_MAX) n = PATH_MAX;

	/* Use the user_fullpath variable for temporary storage */
	memcpy(user_fullpath, sp, n);

	if ((sp = memchr(user_fullpath, '\n', n)) == NULL) /* must be a proper line */
		return(ENOEXEC);

	/* Move sp backwards through script[], prepending each string to stack. */
	for (;;) {
		/* skip spaces behind argument. */
		while (sp > user_fullpath && (*--sp == ' ' || *sp == '\t'));

		if (sp == user_fullpath) break;

		sp[1] = 0;

		/* Move to the start of the argument. */
		while (sp > user_fullpath && sp[-1] != ' ' && sp[-1] != '\t') --sp;

		interp = sp;

		if (!insert_arg(stack, stk_bytes, sp, INSERT)) return(ENOMEM);
	}

	/* Round *stk_bytes up to the size of a pointer for alignment contraints. */
	*stk_bytes= ((*stk_bytes + PTRSIZE - 1) / PTRSIZE) * PTRSIZE;

	if (interp != user_fullpath)
		memmove(user_fullpath, interp, strlen(interp)+1);

	return(OK);
}

/*===========================================================================*
 *				insert_arg				     *
 *===========================================================================*/
static int insert_arg(stack, stk_bytes, arg, replace)
char stack[ARG_MAX];		/* pointer to stack image within PM */
vir_bytes *stk_bytes;		/* size of initial stack */
char *arg;			/* argument to prepend/replace as new argv[0] */
int replace;
{
/* Patch the stack so that arg will become argv[0].  Be careful, the stack may
 * be filled with garbage, although it normally looks like this:
 * nargs argv[0] ... argv[nargs-1] NULL envp[0] ... NULL
 * followed by the strings "pointed" to by the argv[i] and the envp[i].  The
 * pointers are really offsets from the start of stack.
 * Return true iff the operation succeeded.
 */
	int offset, a0, a1, old_bytes = *stk_bytes;

	/* Prepending arg adds at least one string and a zero byte. */
	offset = strlen(arg) + 1;

	a0 = (int) ((char **) stack)[1];	/* argv[0] */

	if (a0 < 4 * PTRSIZE || a0 >= old_bytes) return(FALSE);

	a1 = a0;	/* a1 will point to the strings to be moved */

	if (replace) {
		/* Move a1 to the end of argv[0][] (argv[1] if nargs > 1). */
		do {
			if (a1 == old_bytes) return(FALSE);
			--offset;
		} while (stack[a1++] != 0);
	} else {
		offset += PTRSIZE;	/* new argv[0] needs new pointer in argv[] */
		a0 += PTRSIZE;		/* location of new argv[0][]. */
	}

	/* stack will grow by offset bytes (or shrink by -offset bytes) */
	if ((*stk_bytes += offset) > ARG_MAX) return(FALSE);

	/* Reposition the strings by offset bytes */
	memmove(stack + a1 + offset, stack + a1, old_bytes - a1);

	strcpy(stack + a0, arg);	/* Put arg in the new space. */

	if (!replace) {
		/* Make space for a new argv[0]. */
		memmove(stack + 2 * PTRSIZE, stack + 1 * PTRSIZE, a0 - 2 * PTRSIZE);

		((char **) stack)[0]++;	/* nargs++; */
	}

	/* Now patch up argv[] and envp[] by offset. */
	patch_ptr(stack, (vir_bytes) offset);

	((char **) stack)[1] = (char *) a0;	/* set argv[0] correctly */

	return(TRUE);
}

/*===========================================================================*
 *				patch_ptr				     *
 *===========================================================================*/
static void patch_ptr(stack, base)
char stack[ARG_MAX];		/* pointer to stack image within FS */
vir_bytes base;			/* virtual address of stack base inside user */
{
/* When doing an exec(name, argv, envp) call, the user builds up a stack
 * image with arg and env pointers relative to the start of the stack.  Now
 * these pointers must be relocated, since the stack is not positioned at
 * address 0 in the user's address space.
 */

	char **ap, flag;
	vir_bytes v;

	flag = 0;			/* counts number of 0-pointers seen */
	ap = (char **) stack;		/* points initially to 'nargs' */
	ap++;				/* now points to argv[0] */

	while (flag < 2) {
		if (ap >= (char **) &stack[ARG_MAX]) return;	/* too bad */

		if (*ap != NULL) {
			v = (vir_bytes) *ap;	/* v is relative pointer */
			v += base;		/* relocate it */
			*ap = (char *) v;	/* put it back */
		} else {
			flag++;
		}

		ap++;
	}
}

/**
 * @brief Handle close on exec
 * @param rfp  pointer to fproc structure
 */
static void clo_exec(struct fproc *rfp)
{
/* Files can be marked with the FD_CLOEXEC bit (in fp->fp_cloexec). */
	int i;

	/* Check the file desriptors one by one for presence of FD_CLOEXEC. */
	for (i = 0; i < OPEN_MAX; i++)
		if (FD_ISSET(i, &rfp->fp_cloexec_set))
			close_fd(rfp, i);
}
