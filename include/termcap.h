/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#ifndef _TERMCAP_H
#define _TERMCAP_H

#include <ansi.h>

_PROTOTYPE( int tgetent, (char *_bp, char *_name)			);
_PROTOTYPE( int tgetflag, (char *_id)					);
_PROTOTYPE( int tgetnum, (char *_id)					);
_PROTOTYPE( char *tgetstr, (char *_id, char **_area)			);
_PROTOTYPE( char *tgoto, (char *_cm, int _destcol, int _destline)	);
_PROTOTYPE( int tputs, (char *_cp, int _affcnt, void (*_outc)(int))	);

#endif /* _TERMCAP_H */
