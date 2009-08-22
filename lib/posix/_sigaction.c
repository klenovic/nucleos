/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <lib.h>
#define sigaction _sigaction
#include <asm/sigcontext.h>
#include <signal.h>

int __sigreturn(void);

int sigaction(sig, act, oact)
int sig;
const struct sigaction *act;
struct sigaction *oact;
{
  message m;

  m.m1_i2 = sig;

  /* XXX - yet more type puns because message struct is short of types. */
  m.m1_p1 = (char *) act;
  m.m1_p2 = (char *) oact;
  m.m1_p3 = (char *) __sigreturn;

  return(_syscall(MM, SIGACTION, &m));
}
