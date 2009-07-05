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

static int aout_check_binfmt(struct nucleos_binprm *param, struct vnode *vp);
static int aout_load_binary(struct nucleos_binprm *param);
static int aout_read_seg(struct vnode *vp, off_t off, int proc_e, int seg, phys_bytes seg_bytes);
static int aout_exec_newmem(vir_bytes *stack_topp, int *load_textp, int *allow_setuidp,
			    int proc_e, struct exec_newmem *ex);

struct nucleos_binfmt binfmt_aout = {
	.id = BINFMT_AOUT,
	.check_binfmt = aout_check_binfmt,
	.load_binary = aout_load_binary,
};

/**
 * @brief Check whether it is aout format
 * @param vp[in]  pointer inode for reading exec file
 * @return 0 on success
 */
static int aout_check_binfmt(struct nucleos_binprm *param, struct vnode *vp)
{
	/* Read the header and check the magic number.  The standard MINIX header
	 * is defined in <nucleos/a.out.h>.  It consists of 8 chars followed by 6 longs.
	 * Then come 4 more longs that are not used here.
	 *	Byte 0: magic number 0x01
	 *	Byte 1: magic number 0x03
	 *	Byte 2: normal = 0x10 (not checked, 0 is OK), separate I/D = 0x20
	 *	Byte 3: CPU type, Intel 16 bit = 0x04, Intel 32 bit = 0x10,
	 *            Motorola = 0x0B, Sun SPARC = 0x17
	 *	Byte 4: Header length = 0x20
	 *	Bytes 5-7 are not used.
	 *
	 *	Now come the 6 longs
	 *	Bytes  8-11: size of text segments in bytes
	 *	Bytes 12-15: size of initialized data segment in bytes
	 *	Bytes 16-19: size of bss in bytes
	 *	Bytes 20-23: program entry point
	 *	Bytes 24-27: total memory allocated to program (text, data + stack)
	 *	Bytes 28-31: size of symbol table in bytes
	 * The longs are represented in a machine dependent order,
	 * little-endian on the 8088, big-endian on the 68000.
	 * The header is followed directly by the text and data segments, and the 
	 * symbol table (if any). The sizes are given in the header. Only the 
	 * text and data segments are copied into memory by exec. The header is 
	 * used here only. The symbol table is for the benefit of a debugger and 
	 * is ignored here.
	 */
	off_t pos;
	int err;
	u64_t new_pos;
	unsigned int cum_io_incr;
	struct exec hdr;

	/* Read from the start of the file */
	pos = 0;

	/* Issue request */
	err = req_readwrite(vp->v_fs_e, vp->v_inode_nr, vp->v_index, cvul64(pos), READING, 
			  FS_PROC_NR, (char*)&hdr, sizeof(struct exec), &new_pos, &cum_io_incr);

	if (err) {
		app_err("Can't read the file header\n");
		return -1;
	}

	/* Interpreted script? */
	if (((char*)&hdr)[0] == '#' && ((char*)&hdr)[1] == '!' && vp->v_size >= 2) {
		return ESCRIPT;
	}

	if (vp->v_size < A_MINHDR)
		return -1;

	if (BADMAG(hdr))
		return -1;

#ifdef CONFIG_X86_32
	if (hdr.a_cpu != A_I8086 && hdr.a_cpu != A_I80386)
		return -1;
#endif

	if ((hdr.a_flags & ~(A_NSYM | A_EXEC | A_SEP)) != 0)
		return -1;

	memcpy(param->buf, &hdr, hdr.a_hdrlen);
	param->vp = vp;

	return BINFMT_AOUT;
}

/**
 * @brief Load binary into the memory
 * @param param  parameters for binary format
 * @param ex  memory map structure
 * @return 0 on success
 */
static int aout_load_binary(struct nucleos_binprm *param)
{
	struct exec *hdr;
	off_t off;
	int err = 0;

	hdr = (struct exec*)param->buf;

	param->ex.sep_id = !!(hdr->a_flags & A_SEP);	/* separate I & D or not */

	/* Get text and data sizes. */
	param->ex.text_bytes = (vir_bytes) hdr->a_text;	/* text size in bytes */
	param->ex.data_bytes = (vir_bytes) hdr->a_data;	/* data size in bytes */
	param->ex.bss_bytes  = (vir_bytes) hdr->a_bss;	/* bss size in bytes */
	param->ex.tot_bytes  = hdr->a_total;		/* total bytes to allocate for prog */

	if (param->ex.tot_bytes == 0)
		return -1;

	if (!param->ex.sep_id) {
		/* If I & D space is not separated, it is all considered data. Text=0 */
		param->ex.data_bytes += param->ex.text_bytes;
		param->ex.text_bytes = 0;
	}

	err = aout_exec_newmem(&param->stack_top, &param->load_text, &param->allow_setuid,
			       param->proc_e, &param->ex);

	if (err) {
		app_err("exec failed!\n");
		return err;
	}

	off = hdr->a_hdrlen;

	/* Read in text and data segments. */
	if (param->load_text) {
		err = aout_read_seg(param->vp, off, param->proc_e, T, param->ex.text_bytes);
	}

	off += param->ex.text_bytes;

	if (!err)
		err = aout_read_seg(param->vp, off, param->proc_e, D, param->ex.data_bytes);

	return err;
}

/**
 * @brief Read segment
 * @param vp  inode descriptor to read from
 * @param off  offset in file
 * @param proc_e  process number (endpoint)
 * @param seg  T, D, or S
 * @param seg_bytes  how much is to be transferred?
 * @return 0 on success
 */
static int aout_read_seg(struct vnode *vp, off_t off, int proc_e, int seg, phys_bytes seg_bytes)
{
/* The byte count on read is usually smaller than the segment count, because
 * a segment is padded out to a click multiple, and the data segment is only
 * partially initialized.
 */
	int err;
	unsigned n, k;
	u64_t new_pos;
	unsigned int cum_io_incr;
	char buf[1024];

	/* Make sure that the file is big enough */
	if (vp->v_size < off+seg_bytes) return EIO;

	if (seg != D) {
		/* We have to use a copy loop until safecopies support segments */
		k = 0;
		while (k < seg_bytes) {
			n= seg_bytes - k;

			if (n > sizeof(buf))
				n= sizeof(buf);

#if CONFIG_DEBUG_VFS_AOUT
			printf("read_seg for user %d, seg %d: buf 0x%x, size %d, pos %d\n",
			proc_e, seg, buf, n, off+k);
#endif

			/* Issue request */
			err = req_readwrite(vp->v_fs_e, vp->v_inode_nr, vp->v_index,
					  cvul64(off+k), READING, FS_PROC_NR, buf, n, &new_pos,
					  &cum_io_incr);

			if (err) return err;

			if (cum_io_incr != n) {
				printf("read_seg segment has not been read properly by exec()\n");
				return EIO;
			}

			err = sys_vircopy(FS_PROC_NR, D, (vir_bytes)buf, proc_e, seg, k, n);

			if (err)
				return err;

			k += n;
		}

		return 0;
	}

#if 0
	printf("read_seg for user %d, seg %d: buf 0x%x, size %d, pos %d\n",
	proc_e, seg, 0, seg_bytes, off);
#endif

	/* Issue request */
	err = req_readwrite(vp->v_fs_e, vp->v_inode_nr, vp->v_index, cvul64(off),
			    READING, proc_e, 0, seg_bytes, &new_pos, &cum_io_incr);

	if (err) return err;

	if (!err && cum_io_incr != seg_bytes)
		printf("VFSread_seg segment has not been read properly by exec() \n");

	return err;
}

/**
 * @brief Exec new memory map for a process
 * @param stack_topp[out]  top of the stack
 * @param load_textp[out]  load text segment (otherwise present)
 * @param allow_setuidp[out]  setuid execution is allowed (update uid and gid fields)
 * @param proc_e[in]  process number (endpoint)
 * @param ex[in]  pointer exec structure
 */
static int aout_exec_newmem(vir_bytes *stack_topp, int *load_textp, int *allow_setuidp,
			    int proc_e, struct exec_newmem *ex)
{
	int err;
	message m;

	m.m_type = EXEC_NEWMEM;
	m.EXC_NM_PROC = proc_e;
	m.EXC_NM_PTR = (char *)ex;

	err = sendrec(PM_PROC_NR, &m);

	if (err)
		return err;
#if CONFIG_DEBUG_VFS_AOUT
	printf("exec_newmem: err = %d, m_type = %d\n", err, m.m_type);
#endif
	*stack_topp= m.m1_i1;
	*load_textp= !!(m.m1_i2 & EXC_NM_RF_LOAD_TEXT);
	*allow_setuidp= !!(m.m1_i2 & EXC_NM_RF_ALLOW_SETUID);
#if CONFIG_DEBUG_VFS_AOUT
	printf("exec_newmem: stack_top = 0x%x\n", *stack_topp);
	printf("exec_newmem: load_text = %d\n", *load_textp);
#endif
	return m.m_type;
}
