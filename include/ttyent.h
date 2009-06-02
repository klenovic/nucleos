/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
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

#ifndef _ANSI_H
#include <ansi.h>
#endif

struct ttyent {
	char	*ty_name;	/* Name of the terminal device. */
	char	*ty_type;	/* Terminal type name (termcap(3)). */
	char	**ty_getty;	/* Program to run, normally getty. */
	char	**ty_init;	/* Initialization command, normally stty. */
};

_PROTOTYPE( struct ttyent *getttyent, (void)				);
_PROTOTYPE( struct ttyent *getttynam, (const char *_name)		);
_PROTOTYPE( int setttyent, (void)					);
_PROTOTYPE( void endttyent, (void)					);

#endif /* _TTYENT_H */
