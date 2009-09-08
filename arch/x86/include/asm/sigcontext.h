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

/* The sigcontext structure is used by the sigreturn(2) system call.
 * sigreturn() is seldom called by user programs, but it is used internally
 * by the signal catching mechanism.
 */

/* The following structure should match the stackframe_s structure used
 * by the kernel's context switching code.  Floating point registers should
 * be added in a different struct.
 */
struct sigregs {
  short sr_gs;
  short sr_fs;
  short sr_es;
  short sr_ds;
  int sr_di;
  int sr_si;
  int sr_bp;
  int sr_st;			/* stack top -- used in kernel */
  int sr_bx;
  int sr_dx;
  int sr_cx;
  int sr_retreg;
  int sr_retadr;		/* return address to caller of save -- used
  				 * in kernel */
  int sr_pc;
  int sr_cs;
  int sr_psw;
  int sr_sp;
  int sr_ss;
};

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
  int sc_flags;			/* sigstack state to restore */
  long sc_mask;			/* signal mask to restore */
  struct sigregs sc_regs;	/* register set to restore */
};

#define sc_gs sc_regs.sr_gs
#define sc_fs sc_regs.sr_fs
#define sc_es sc_regs.sr_es
#define sc_ds sc_regs.sr_ds
#define sc_di sc_regs.sr_di
#define sc_si sc_regs.sr_si 
#define sc_fp sc_regs.sr_bp
#define sc_st sc_regs.sr_st		/* stack top -- used in kernel */
#define sc_bx sc_regs.sr_bx
#define sc_dx sc_regs.sr_dx
#define sc_cx sc_regs.sr_cx
#define sc_retreg sc_regs.sr_retreg
#define sc_retadr sc_regs.sr_retadr	/* return address to caller of 
					save -- used in kernel */
#define sc_pc sc_regs.sr_pc
#define sc_cs sc_regs.sr_cs
#define sc_psw sc_regs.sr_psw
#define sc_sp sc_regs.sr_sp
#define sc_ss sc_regs.sr_ss

int sigreturn(struct sigcontext *_scp);

#endif /* __ASM_X86_SIGCONTEXT_H */
