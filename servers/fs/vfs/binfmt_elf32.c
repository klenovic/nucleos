/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include "fs.h"
#include <stdlib.h>
#include <nucleos/stat.h>
#include <nucleos/kernel.h>
#include <nucleos/unistd.h>
#include <nucleos/endpoint.h>
#include <nucleos/com.h>
#include <nucleos/u64.h>
#include <nucleos/elf.h>
#include <nucleos/binfmts.h>
#include <nucleos/vfsif.h>
#include <nucleos/elf.h>
#include <nucleos/signal.h>
#include <nucleos/string.h>
#include <nucleos/dirent.h>
#include <servers/vfs/fproc.h>
#include <asm/page.h>

#include "param.h"
#include "vnode.h"
#include "vmnt.h"

/** @brief Default stack size */
#define STACKSIZE	CONFIG_VFS_ELF32_STACKSIZE
#define ELF_PAGEALIGN(a) (((a) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))

typedef Elf32_Ehdr elf32_ehdr_t;
typedef Elf32_Phdr elf32_phdr_t;
typedef Elf32_Shdr elf32_shdr_t;

/** @brief Exec info */
typedef struct elf32_exec_info {
	uint32_t offset_next;
	uint32_t text_pstart;
	uint32_t text_vstart;
	uint32_t text_size;
	uint32_t data_pstart;
	uint32_t data_vstart;
	uint32_t data_size;
	uint32_t bss_pstart;
	uint32_t bss_vstart;
	uint32_t bss_size;
	uint32_t entry_point;
	uint32_t symtab_size;
	uint32_t strtab_size;
	uint8_t sep_id;
} elf32_exec_info_t;

static int elf32_check_binfmt(struct nucleos_binprm *param, struct vnode *vp);
static int elf32_load_binary(struct nucleos_binprm *param);
static int elf32_read_seg(struct vnode *vp, off_t off, vir_bytes addr_off, int proc_e, int seg,
			  phys_bytes seg_bytes);
static int elf32_map_exec(elf32_exec_info_t *exec, elf32_ehdr_t *ehdr, elf32_phdr_t *phdr,
			  elf32_shdr_t *shdr);
static int elf32_exec_newmem(vir_bytes *stack_topp, int *load_textp, int *allow_setuidp,
			     int proc_e, struct exec_newmem *ex);

int elf32_read_ehdr(elf32_ehdr_t *ehdr, struct vnode *vp);
int elf32_read_phdrs(elf32_phdr_t **phdrs, elf32_ehdr_t *ehdr, struct vnode *vp);
int elf32_read_shdrs(elf32_shdr_t **shdrs, elf32_ehdr_t *ehdr, struct vnode *vp);

#ifdef CONFIG_DEBUG_VFS_ELF32
/* helper dump functions */
static void elf32_dump_ehdr(elf32_ehdr_t* eh);
static void elf32_dump_phdr(elf32_phdr_t* ph);
static void elf32_dump_shdr(elf32_shdr_t* sh);
static void elf32_dump_exec(elf32_exec_info_t* e);
static void elf32_dump_exec_newmem(struct exec_newmem *ex);
#endif

struct nucleos_binfmt binfmt_elf32 = {
	.id = BINFMT_ELF,
	.check_binfmt = elf32_check_binfmt,
	.load_binary = elf32_load_binary,
};

/**
 * @brief Check whether it is aout format
 * @param param[out]  pointer to binary parameter structure
 * @param vp[in]  pointer inode for reading exec file
 * @return 0 on success
 */
static int elf32_check_binfmt(struct nucleos_binprm *param, struct vnode *vp)
{
	off_t pos;
	int err;
	u64_t new_pos;
	unsigned int cum_io;
	elf32_ehdr_t hdr;

	/* Read from the start of the file */
	pos = 0;

	/* Issue request */
	err = req_readwrite(vp->v_fs_e, vp->v_inode_nr, cvul64(pos), READING,
			    VFS_PROC_NR, (char*)&hdr, sizeof(elf32_ehdr_t), &new_pos, &cum_io);

	if (err) {
		app_err("Can't read the file header\n");
		return -1;
	}
	
	/* is it elf? */
	if (hdr.e_ident[EI_MAG0] != ELFMAG0 ||
	    hdr.e_ident[EI_MAG1] != ELFMAG1 ||
	    hdr.e_ident[EI_MAG2] != ELFMAG2 ||
	    hdr.e_ident[EI_MAG3] != ELFMAG3) {
		/* it's not an ELF file */
		return -1;
	}

	/* class 32? */
	if (hdr.e_ident[EI_CLASS] != ELFCLASS32) {
		app_err("Only 32-bit ELF is supported for now!\n")
		return -1;
	}

	if (hdr.e_machine != EM_386) {
		app_err("Only x86 architecure is supported for now!\n")
		return -1;
	}

	/* Save the header for further use */
	memcpy(param->buf, &hdr, sizeof(elf32_ehdr_t));
	param->vp = vp;

	/* header checked */
	param->checked = 1;

	return BINFMT_ELF;
}

/**
 * @brief Load binary into the memory
 * @param param  parameters for binary format
 * @return 0 on success
 */
static int elf32_load_binary(struct nucleos_binprm *param)
{
	int i = 0;
	int err = 0;
	elf32_exec_info_t exec;
	elf32_ehdr_t ehdr;
	elf32_phdr_t *phdrs = 0;
	elf32_shdr_t *shdrs = 0;

	/* Read in all headers. */
	if (!param->checked) {
		app_err("Unchecked ELF header\n");
		return -1;
	}

	/* initialization */
	memset(&exec, 0, sizeof(elf32_exec_info_t));

	/* The ehdr was read into buffer during check */
	memcpy(&ehdr, param->buf, sizeof(elf32_ehdr_t));

	err = elf32_read_phdrs(&phdrs, &ehdr, param->vp);
	if (err) {
		app_err("Can't read program headers\n");
		return -1;
	}

	err = elf32_read_shdrs(&shdrs, &ehdr, param->vp);
	if (err) {
		app_err("Can't read section headers\n");
		return -1;
	}

#ifdef CONFIG_DEBUG_VFS_ELF32
	printk("---[ELF header]---\n");
	elf32_dump_ehdr(&ehdr);

	for(i=0; i<ehdr.e_phnum; i++) {
		printk("---[%d. program header]---\n",i);
		elf32_dump_phdr(&phdrs[i]);
	}

	for(i=0; i<ehdr.e_shnum; i++) {
		printk("---[%d. section header]---\n",i);
		elf32_dump_shdr(&shdrs[i]);
	}
#endif
	/* map on exec structure */
	elf32_map_exec(&exec, &ehdr, phdrs, shdrs);

#ifdef CONFIG_DEBUG_VFS_ELF32
	elf32_dump_exec(&exec);
#endif

	param->ex.sep_id = exec.sep_id;	/* separate I & D or not */

	/* Get text and data sizes. */
	param->ex.text_bytes = (vir_bytes) exec.text_size;	/* text size in bytes */
	param->ex.data_bytes = (vir_bytes) exec.data_size;	/* data size in bytes */
	param->ex.bss_bytes  = (vir_bytes) exec.bss_size;	/* bss size in bytes */

	/* compute total bytes to allocate for prog */
	if (exec.sep_id)
		param->ex.tot_bytes  = exec.data_size + exec.bss_size + STACKSIZE;
	else {
		param->ex.tot_bytes  = exec.text_size + exec.data_size + exec.bss_size + STACKSIZE;

		/* If I & D space is not separated, it is all considered data. Text = 0 */
		param->ex.data_bytes += exec.text_size;
		param->ex.text_bytes = 0;
	}	

	/* entry point of process */
	param->ex.entry_point = exec.entry_point;

	if (param->ex.tot_bytes == 0)
		return -1;

#ifdef CONFIG_DEBUG_VFS_ELF32
	elf32_dump_exec_newmem(&param->ex);
#endif

	/* allocate memory for the new process */
	err = elf32_exec_newmem(&param->stack_top, &param->load_text, &param->allow_setuid,
				param->proc_e, &param->ex);

	if (err) {
		app_err("Can't allocate memory for process\n");
		return err;
	}

	/* address inside process (important in case of common I&D) */
	vir_bytes addr_off = 0;
	elf32_shdr_t* sh = 0;

	/* Read in text and data segments. */
	for (i = 0; i < ehdr.e_shnum; i++) {
		sh = &shdrs[i];
		addr_off = sh->sh_addr;

		if (param->load_text && sh->sh_type == SHT_PROGBITS) {
			if ((sh->sh_flags & SHF_ALLOC) &&
			    ((sh->sh_flags & SHF_EXECINSTR) || (~sh->sh_flags & SHF_WRITE)))  {
				/* .text and read-only sections */
				err = elf32_read_seg(param->vp, sh->sh_offset, addr_off, param->proc_e,
						     (exec.sep_id) ? T : D , sh->sh_size);
				if (err) {
					app_err("Can't load text section\n");
					return err;
				}
#ifdef CONFIG_DEBUG_VFS_ELF32
				app_dbg("text sections loaded\n");
#endif
			} else if ((sh->sh_flags & SHF_ALLOC) && (sh->sh_flags & SHF_WRITE)) {
				/* data sections */
				err = elf32_read_seg(param->vp, sh->sh_offset, addr_off,
						     param->proc_e, D, sh->sh_size);
				if (err) {
					app_err("Can't load data section\n");
					return err;
				}
#ifdef CONFIG_DEBUG_VFS_ELF32
				app_dbg("sata sections loaded\n");
#endif
			}
		}
	}

#ifdef CONFIG_DEBUG_VFS_ELF32
	app_dbg("%s loaded\n",param->ex.progname);
#endif

	free(phdrs);
	free(shdrs);

	return 0;
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
static int elf32_read_seg(struct vnode *vp, off_t off, vir_bytes addr_off, int proc_e, int seg,
			  phys_bytes seg_bytes)
{
/* The byte count on read is usually smaller than the segment count, because
 * a segment is padded out to a click multiple, and the data segment is only
 * partially initialized.
 */
	int err;
	unsigned n, k;
	u64_t new_pos;
	unsigned int cum_io;
	char buf[1024];

	/* Make sure that the file is big enough */
	if (vp->v_size < off + seg_bytes) return -EIO;

	if (seg != D) {
		/* We have to use a copy loop until safecopies support segments */
		k = 0;
		while (k < seg_bytes) {
			n = seg_bytes - k;

			if (n > sizeof(buf))
				n = sizeof(buf);
#ifdef CONFIG_DEBUG_VFS_ELF32
			printk("read_seg for user %d, seg %d: buf 0x%x, size %d, pos %d\n",
			proc_e, seg, buf, n, off+k);
#endif
			/* Issue request */
			err = req_readwrite(vp->v_fs_e, vp->v_inode_nr,
					    cvul64(off + k), READING, VFS_PROC_NR, buf, n, &new_pos,
					    &cum_io);

			if (err) {
				printk("VFS: read_seg: req_readwrite failed (text)\n");
				return err;
			}

			if (cum_io != n) {
				app_err("Segment 0x%X has not been read properly!\n", seg);
				return -EIO;
			}

			err = sys_vircopy(VFS_PROC_NR, D, (vir_bytes)buf, proc_e, seg, k, n);

			if (err) {
				printk("VFS: read_seg: copy failed (text)\n");
				return err;
			}

			k += n;
		}

		return 0;
	}

#ifdef CONFIG_DEBUG_VFS_ELF32
	printk("read_seg for user %d, seg %d: buf 0x%x, size %d, pos %d\n",
	proc_e, seg, addr_off, seg_bytes, off);
#endif

	/* Issue request */
	err = req_readwrite(vp->v_fs_e, vp->v_inode_nr, cvul64(off),
			    READING, proc_e, (char*)addr_off, seg_bytes, &new_pos, &cum_io);

	if (err) {
		printk("VFS: read_seg: req_readwrite failed (data)\n");
		return err;
	}

	if (!err && cum_io != seg_bytes)
		app_err("Segment 0x%X has not been read properly!\n", seg);

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
static int elf32_exec_newmem(vir_bytes *stack_topp, int *load_textp, int *allow_setuidp,
			     int proc_e, struct exec_newmem *ex)
{
	int err;
	kipc_msg_t m;

	m.m_type = KCNR_EXEC_NEWMEM;
	m.EXC_NM_PROC = proc_e;
	m.EXC_NM_PTR = (char *)ex;

	err = kipc_module_call(KIPC_SENDREC, 0, PM_PROC_NR, &m);

	if (err)
		return err;

#ifdef CONFIG_DEBUG_VFS_ELF32
	printk("exec_newmem: err = %d, m_type = %d\n", err, m.m_type);
#endif
	*stack_topp= m.m_data1;
	*load_textp= !!(m.m_data2 & EXC_NM_RF_LOAD_TEXT);
	*allow_setuidp= !!(m.m_data2 & EXC_NM_RF_ALLOW_SETUID);

#ifdef CONFIG_DEBUG_VFS_ELF32
	printk("exec_newmem: stack_top = 0x%x\n", *stack_topp);
	printk("exec_newmem: load_text = %d\n", *load_textp);
#endif
	/* the status is saved in m_type */
	return m.m_type;
}

/**
 * @brief Map headers to exec structure
 * @param exec  pointer exec structure
 * @param ehdr  pointer to ELF header
 * @param phdrs  pointer to ELF program section headers
 * @param shdrs  pointer to ELF sections hea
 * @return 0 on success
 */
static int elf32_map_exec(elf32_exec_info_t *exec, elf32_ehdr_t *ehdr, elf32_phdr_t *phdrs,
			  elf32_shdr_t *shdrs)
{
	int i = 0;

	exec->entry_point = ehdr->e_entry;

	if (ehdr->e_phoff != 0 && ehdr->e_shoff == 0) {
		/*
		 * We only have a program headers.  Walk all program headers
		 * and try to figure out what the sections are.
		 */
		for (i = 0; i < ehdr->e_phnum; i++) {
			elf32_phdr_t* ph = &phdrs[i];

			/* Assume that a segment without write
			 * permissions is .text and read-only sections
			 */
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
	/* @nucleos: The current kernel expects to start the .text section
	 *           from 0, thing to change.
	 */
	unsigned int tlow = 0, thigh = 0;
	unsigned int dlow = ~0UL, dhigh = 0;
	unsigned int blow = ~0UL, bhigh = 0;

	for (i = 0; i < ehdr->e_shnum; i++) {
		elf32_shdr_t* sh = &shdrs[i];

		if (sh->sh_type == SHT_PROGBITS) {
			/* .text and read-only segments */
			if ((sh->sh_flags & SHF_ALLOC) &&
			    ((sh->sh_flags & SHF_EXECINSTR) || (~sh->sh_flags & SHF_WRITE))) {
				/* @nucleos: This check is useless for now since
				 *           it was set to 0.
				 */
				if (sh->sh_addr < tlow)
					tlow = sh->sh_addr;

				if (sh->sh_addr + sh->sh_size > thigh)
					thigh = sh->sh_addr + sh->sh_size;

			} else if ((sh->sh_flags & SHF_ALLOC) && (sh->sh_flags & SHF_WRITE)) {
				/* sections .data */
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
		} else if (sh->sh_type == SHT_SYMTAB) {
			/* Symbol table */
			exec->symtab_size = sh->sh_size;
		} else if (sh->sh_type == SHT_STRTAB) {
			/* String table */
			exec->strtab_size = sh->sh_size;
		}
	}

	/*
	 * Translate the virtual addresses of the sections to physical
	 * addresses using the segments in the program header.
	 */
#define INSEGMENT(a)	(a >= ph->p_vaddr && a < (ph->p_vaddr + ph->p_memsz))
#define PADDR(a)	(ph->p_paddr + a - ph->p_vaddr)

	for (i = 0; i < ehdr->e_phnum; i++) {
		elf32_phdr_t* ph = &phdrs[i];

		if (INSEGMENT (tlow)) {
			exec->text_pstart = PADDR (tlow);
			exec->text_vstart = tlow;
			exec->text_size = thigh - tlow;
			exec->text_size = ELF_PAGEALIGN(exec->text_size);
		}

		if (INSEGMENT (dlow)) {
			exec->data_pstart = PADDR (dlow);
			exec->data_vstart = dlow;
			exec->data_size = dhigh - dlow;
			exec->data_size = ELF_PAGEALIGN(exec->data_size);
		}

		if (INSEGMENT (blow)) {
			exec->bss_pstart = PADDR (blow);
			exec->bss_vstart = blow;
			exec->bss_size = bhigh - blow;
			exec->bss_size = ELF_PAGEALIGN(exec->bss_size);
		}
	}

	if (exec->text_vstart == exec->data_vstart)
		exec->sep_id = 1;
	else
		exec->sep_id = 0;

	return 0;
}

/**
 * @brief Read in ELF header
 * @param ehdr[out]  pointer to ELF header
 * @param vp  poiner to inode descriptor to read from
 * @return 0 on success
 */
int elf32_read_ehdr(elf32_ehdr_t *ehdr, struct vnode *vp)
{
	int err;
	u64_t new_pos;
	unsigned int cum_io_incr;

	/* Issue request */
	err = req_readwrite(vp->v_fs_e, vp->v_inode_nr, cvul64(0), READING,
			    VFS_PROC_NR, (char*)&ehdr, sizeof(elf32_ehdr_t), &new_pos, &cum_io_incr);

	if (err)
		return -1;

	if (cum_io_incr != sizeof(elf32_ehdr_t)) {
		app_err("Segment has not been read properly!\n");
		return -EIO;
	}

	return 0;
}

/**
 * @brief Read in ELF program headers
 * @param ehdr[out]  pointer to ELF program headers
 * @param vp  poiner to inode descriptor to read from
 * @return 0 on success
 */
int elf32_read_phdrs(elf32_phdr_t **phdrs, elf32_ehdr_t *ehdr, struct vnode *vp)
{
	u64_t new_pos;
	unsigned int cum_io_incr;
	int err;
	int phdrs_sz = 0;

	if (ehdr->e_phnum)
		phdrs_sz = ehdr->e_phnum*sizeof(elf32_phdr_t);
	else {
		app_err("Missing program segment headers\n");
		return -EINVAL;
	}

	/* allocate space for program headers */
	*phdrs = (elf32_phdr_t*)malloc(phdrs_sz);

	if (!phdrs) {
		app_err("Can't allocate memory for program segment headers");
		return -ENOMEM;
	}

	/* Issue request */
	err = req_readwrite(vp->v_fs_e, vp->v_inode_nr, cvul64(ehdr->e_phoff), READING,
			    VFS_PROC_NR, (char*)*phdrs, phdrs_sz, &new_pos, &cum_io_incr);

	if (err) {
		free(phdrs);
		return -1;
	}

	return 0;
}

/**
 * @brief Read in ELF segment headers
 * @param ehdr[out]  pointer to ELF program headers
 * @param vp  poiner to inode descriptor to read from
 * @return 0 on success
 */
int elf32_read_shdrs(elf32_shdr_t **shdrs, elf32_ehdr_t *ehdr, struct vnode *vp)
{
	u64_t new_pos;
	unsigned int cum_io_incr;
	int err;
	int shdrs_sz = 0;

	if (ehdr->e_shnum)
		shdrs_sz = ehdr->e_shnum*sizeof(elf32_shdr_t);
	else {
		app_err("Missing section headers\n");
		return -1;
	}

	/* allocate space for segment headers */
	*shdrs = (elf32_shdr_t*)malloc(shdrs_sz);

	if (!shdrs) {
		app_err("Can't allocate memory for section headers\n");
		return 0;
	}

	/* Issue request */
	err = req_readwrite(vp->v_fs_e, vp->v_inode_nr, cvul64(ehdr->e_shoff), READING,
			    VFS_PROC_NR, (char*)*shdrs, shdrs_sz, &new_pos, &cum_io_incr);

	if (err) {
		free(shdrs);
		return -1;
	}

	return 0;
}

#ifdef CONFIG_DEBUG_VFS_ELF32
static void elf32_dump_exec(elf32_exec_info_t* e)
{
	printk("text_pstart=0x%x  ", e->text_pstart);
	printk("text_vstart=0x%x  ", e->text_vstart);
	printk("text_size=0x%x\n", e->text_size);
	printk("data_pstart=0x%x  ", e->data_pstart);
	printk("data_vstart=0x%x  ", e->data_vstart);
	printk("data_size=0x%x\n", e->data_size);
	printk("bss_pstart=0x%x  ", e->bss_pstart);
	printk("bss_vstart=0x%x  ", e->bss_vstart);
	printk("bss_size=0x%x\n", e->bss_size);
	printk("entry_point=0x%x  ", e->entry_point);
	printk("sep_id=0x%x\n", e->sep_id);

	return;
}

static void elf32_dump_ehdr(elf32_ehdr_t* eh)
{
	int i=0;

	printk("e_ident: ");
	for(i = 0; i < EI_NIDENT; i++) {
		printk("0x%x ",eh->e_ident[i]);     /* Magic number and other info */
	}
	printk("\n");

	printk("type: 0x%x ", eh->e_type);            /* Object file type */
	printk("machine: 0x%x ", eh->e_machine);      /* Architecture */
	printk("version: 0x%x ", eh->e_version);      /* Object file version */
	printk("entry: 0x%x ", eh->e_entry);          /* Entry point virtual address */
	printk("phoff: 0x%x\n", eh->e_phoff);         /* Program header table file offset */
	printk("shoff: 0x%x ", eh->e_shoff);          /* Section header table file offset */
	printk("flags: 0x%x ", eh->e_flags);          /* Processor-specific flags */
	printk("ehsize: 0x%x ", eh->e_ehsize);        /* ELF header size in bytes */
	printk("phentsize: 0x%x\n", eh->e_phentsize); /* Program header table entry size */
	printk("phnum: 0x%x ", eh->e_phnum);          /* Program header table entry count */
	printk("shentsize: 0x%x ", eh->e_shentsize);  /* Section header table entry size */
	printk("shnum: 0x%x ", eh->e_shnum);          /* Section header table entry count */
	printk("shstrndx: 0x%x\n", eh->e_shstrndx);   /* Section header string table index */

	return;
}

static void elf32_dump_phdr(elf32_phdr_t* ph)
{
	printk("type: 0x%x ", ph->p_type);     /* Segment type */
	printk("offset: 0x%x ", ph->p_offset); /* Segment file offset */
	printk("vaddr: 0x%x ", ph->p_vaddr);   /* Segment virtual address */
	printk("paddr: 0x%x\n", ph->p_paddr);  /* Segment physical address */
	printk("filesz: 0x%x ", ph->p_filesz); /* Segment size in file */
	printk("memsz: 0x%x ", ph->p_memsz);   /* Segment size in memory */
	printk("flags: 0x%x ", ph->p_flags);   /* Segment flags */
	printk("align: 0x%x\n", ph->p_align);  /* Segment alignment */

	return;
}

static void elf32_dump_shdr(elf32_shdr_t* sh)
{
	printk("name: 0x%x ", sh->sh_name);           /* Section name (string tbl index) */
	printk("type: 0x%x ", sh->sh_type);           /* Section type */
	printk("flags: 0x%x ", sh->sh_flags);         /* Section flags */
	printk("addr: 0x%x ", sh->sh_addr);           /* Section virtual addr at execution */
	printk("offset: 0x%x\n", sh->sh_offset);      /* Section file offset */
	printk("size: 0x%x ", sh->sh_size);           /* Section size in bytes */
	printk("link: 0x%x ", sh->sh_link);           /* Link to another section */
	printk("info: 0x%x ", sh->sh_info);           /* Additional section information */
	printk("addralign: 0x%x ", sh->sh_addralign); /* Section alignment */
	printk("entsize: 0x%x\n", sh->sh_entsize);    /* Entry size if section holds table */

	return;
}

static void elf32_dump_exec_newmem(struct exec_newmem *ex)
{
	printk("text_bytes: 0x%x ", ex->text_bytes);
	printk("data_bytes: 0x%x ", ex->data_bytes);
	printk("bss_bytes: 0x%x ", ex->bss_bytes);
	printk("tot_bytes: 0x%x ", ex->tot_bytes);
	printk("args_bytes: 0x%x\n", ex->args_bytes);
	printk("sep_id: 0x%x ", ex->sep_id);
	printk("st_dev: 0x%x ", ex->st_dev);
	printk("st_ino: 0x%x ", ex->st_ino);
	printk("st_ctime: 0x%x ", ex->st_ctime);
	printk("new_uid: 0x%x\n", ex->new_uid);
	printk("new_gid: 0x%x ", ex->new_gid);
	printk("progname: %s\n", ex->progname);

	return;
}
#endif /* CONFIG_DEBUG_VFS_ELF32 */
