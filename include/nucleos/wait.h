/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The <nucleos/wait.h> header contains macros related to wait(). The value
 * returned by wait() and waitpid() depends on whether the process 
 * terminated by an exit() call, was killed by a signal, or was stopped
 * due to job control, as follows:
 *
 *				 High byte   Low byte
 *				+---------------------+
 *	exit(status)		|  status  |    0     |
 *				+---------------------+
 *      killed by signal	|    0     |  signal  |
 *				+---------------------+
 *	stopped (job control)	|  signal  |   0177   |
 *				+---------------------+
 */

#ifndef _NUCLEOS_WAIT_H
#define _NUCLEOS_WAIT_H

#define WNOHANG		1	/* do not wait for child to exit */
#define WUNTRACED	2	/* for job control; not implemented */

#if defined(__KERNEL__) || defined(__UKERNEL__)

#include <nucleos/types.h>

#define WIFEXITED(w)	(((w) & 0xff) == 0)				/* normal exit */
#define WIFSIGNALED(w)	(((w) & 0x7f) > 0 && (((w) & 0x7f) < 0x7f))	/* signaled */
#define WIFSTOPPED(w)	(((w) & 0xff) == 0x7f)				/* stopped */
#define WEXITSTATUS(w)	(((w) >> 8) & 0xff)				/* exit status */
#define WTERMSIG(w)	((w) & 0x7f)					/* sig value */
#define WSTOPSIG(w)	WEXITSTATUS(w)					/* stop signal */

/* Function Prototypes. */
pid_t wait(int *_stat_loc);
pid_t waitpid(pid_t _pid, int *_stat_loc, int _options);
#endif /* defined(__KERNEL__) || defined(__UKERNEL__) */

#endif /* _NUCLEOS_WAIT_H */
