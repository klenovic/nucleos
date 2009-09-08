/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
 * sigmisc.c - used to get a signal mask
 */
/* This can't be done in setjmp.e, since SIG_SETMASK is defined in
 * <nucleos/signal.h>. This is a C-file, which can't be included.
 */
#include <nucleos/types.h>
#include <nucleos/signal.h>
#include <nucleos/stddef.h>

static void __testsigset(void) {
	/* This switch compiles when a sigset_t has the right size. */
	switch(0) {
	case 0: 
	case sizeof(sigset_t) <= sizeof(long): break;
	}
}

void __newsigset(sigset_t *p)
{
	/* The SIG_SETMASK is not significant */
	sigprocmask(SIG_SETMASK, NULL, p);
}

void __oldsigset(sigset_t *p)
{
	sigprocmask(SIG_SETMASK, p, NULL);
}
