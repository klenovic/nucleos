/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef __ASM_GENERIC_SIGNAL_DEFS_H
#define __ASM_GENERIC_SIGNAL_DEFS_H

/* POSIX requires these values for use with sigprocmask(2). */
#ifndef SIG_BLOCK
#define SIG_BLOCK	0	/* for blocking signals */
#endif

#ifndef SIG_UNBLOCK
#define SIG_UNBLOCK	1    /* for unblocking signals */
#endif

#ifndef SIG_SETMASK
#define SIG_SETMASK	2    /* for setting the signal mask */
#endif

#ifndef SIG_INQUIRE
#define SIG_INQUIRE	4    /* for internal use only */
#endif

#ifndef __ASSEMBLY__

typedef void __signalfn_t(int);
typedef __signalfn_t *__sighandler_t;

typedef void __restorefn_t(void);
typedef __restorefn_t *__sigrestore_t;

/* Macros used as function pointers. */
#define SIG_ERR		((__sighandler_t) -1)	/* error return from signal */
#define SIG_DFL		((__sighandler_t)  0)	/* default signal handling */
#define SIG_IGN		((__sighandler_t)  1)	/* ignore signal */

#define SIG_HOLD	((__sighandler_t)  2)   /* block signal */
#define SIG_CATCH	((__sighandler_t)  3)   /* catch signal */
#define SIG_MESS	((__sighandler_t)  4)   /* pass as message (Nucleos) */

#endif /* __ASSEMBLY__ */

#endif /* __ASM_GENERIC_SIGNAL_DEFS_H */
