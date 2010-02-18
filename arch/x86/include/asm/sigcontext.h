/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __ASM_X86_SIGCONTEXT_H
#define __ASM_X86_SIGCONTEXT_H

#include <asm/stackframe.h>
#include <asm/fpu.h>

typedef struct stackframe_s sigregs;

struct sigframe {		/* stack frame created for signalled process */
	void (*sf_retadr)(void);
	int sf_signo;
	int sf_code;
	struct sigcontext *sf_scp;
	int sf_fp;
	void (*sf_retadr2)(void);
	struct sigcontext *sf_scpcopy;
};

struct sigcontext {
	int sc_flags;		/* sigstack state to restore (including
				 * MF_FPU_INITIALIZED)
				 */
	long sc_mask;		/* signal mask to restore */
	sigregs sc_regs;	/* register set to restore */
	union fpu_state_u sc_fpu_state;
};

#define sc_gs sc_regs.gs
#define sc_fs sc_regs.fs
#define sc_es sc_regs.es
#define sc_ds sc_regs.ds
#define sc_di sc_regs.di
#define sc_si sc_regs.si
#define sc_fp sc_regs.bp
#define sc_st sc_regs.st	/* stack top -- used in kernel */
#define sc_bx sc_regs.bx
#define sc_dx sc_regs.dx
#define sc_cx sc_regs.cx
#define sc_retreg sc_regs.retreg
#define sc_retadr sc_regs.retadr	/* return address to caller of
					   save -- used in kernel */
#define sc_pc sc_regs.pc
#define sc_cs sc_regs.cs
#define sc_psw sc_regs.psw
#define sc_sp sc_regs.sp
#define sc_ss sc_regs.ss

int sigreturn(struct sigcontext *_scp);

#endif /* __ASM_X86_SIGCONTEXT_H */
