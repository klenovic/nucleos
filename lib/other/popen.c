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
 * popen - open a pipe
 */
#include <nucleos/types.h>
#include <nucleos/limits.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>

typedef int wait_arg;

#include "../stdio/loc_incl.h"

int _close(int d);
int _dup2(int oldd, int newd);		/* not present in System 5 */
int _execl(const char *name, const char *_arg, ... );
pid_t _fork(void);
int _pipe(int fildes[2]);
pid_t _wait(wait_arg *status);
void _exit(int status);

static int pids[OPEN_MAX];

FILE *
popen(command, type)
const char *command;
const char *type;
{
	int piped[2];
	int Xtype = *type == 'r' ? 0 : *type == 'w' ? 1 : 2;
	int pid;

	if (Xtype == 2 ||
	    _pipe(piped) < 0 ||
	    (pid = _fork()) < 0) return 0;
	
	if (pid == 0) {
		/* child */
		register int *p;

		for (p = pids; p < &pids[OPEN_MAX]; p++) {
			if (*p) _close((int)(p - pids));
		}
		_close(piped[Xtype]);
		_dup2(piped[!Xtype], !Xtype);
		_close(piped[!Xtype]);
		_execl("/bin/sh", "sh", "-c", command, (char *) 0);
		_exit(127);	/* like system() ??? */
	}

	pids[piped[Xtype]] = pid;
	_close(piped[!Xtype]);
	return fdopen(piped[Xtype], type);
}

#define	ret_val	status

int
pclose(stream)
FILE *stream;
{
	int fd = fileno(stream);
	wait_arg status;
	int wret;

	void (*intsave)(int) = signal(SIGINT, SIG_IGN);
	void (*quitsave)(int) = signal(SIGQUIT, SIG_IGN);

	fclose(stream);
	while ((wret = _wait(&status)) != -1) {
		if (wret == pids[fd]) break;
	}
	if (wret == -1) ret_val = -1;
	signal(SIGINT, intsave);
	signal(SIGQUIT, quitsave);
	pids[fd] = 0;
	return ret_val;
}
