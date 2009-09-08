/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <nucleos/lib.h>
#include <asm/sigcontext.h>
#include <nucleos/signal.h>

int sigreturn(register struct sigcontext *scp)
{
  sigset_t set;

  /* The message can't be on the stack, because the stack will vanish out
   * from under us.  The send part of sendrec will succeed, but when
   * a message is sent to restart the current process, who knows what will
   * be in the place formerly occupied by the message?
   */
  static message m;

  /* Protect against race conditions by blocking all interrupts. */
  sigfillset(&set);		/* splhi */
  sigprocmask(SIG_SETMASK, &set, (sigset_t *) NULL);

  m.m2_l1 = scp->sc_mask;
  m.m2_i2 = scp->sc_flags;
  m.m2_p1 = (char *) scp;
  return(_syscall(PM_PROC_NR, __NR_sigreturn, &m));	/* normally this doesn't return */
}
