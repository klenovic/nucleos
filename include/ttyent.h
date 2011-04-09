/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* <ttyent.h> is used by getttyent(3).			Author: Kees J. Bot
 *								28 Oct 1995
 */
#ifndef _TTYENT_H
#define _TTYENT_H

struct ttyent {
	char	*ty_name;	/* Name of the terminal device. */
	char	*ty_type;	/* Terminal type name (termcap(3)). */
	char	**ty_getty;	/* Program to run, normally getty. */
	char	**ty_init;	/* Initialization command, normally stty. */
};

struct ttyent *getttyent(void);
struct ttyent *getttynam(const char *_name);
int setttyent(void);
void endttyent(void);

#endif /* _TTYENT_H */
