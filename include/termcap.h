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

int tgetent(char *_bp, char *_name);
int tgetflag(char *_id);
int tgetnum(char *_id);
char *tgetstr(char *_id, char **_area);
char *tgoto(char *_cm, int _destcol, int _destline);
int tputs(char *_cp, int _affcnt, void (*_outc)(int));

#endif /* _TERMCAP_H */
