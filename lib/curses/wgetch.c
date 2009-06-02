/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <curses.h>
#include <stdio.h>
#include "curspriv.h"

int wgetch(win)
WINDOW *win;
{
  bool weset = FALSE;
  char inp;

  if (!win->_scroll && (win->_flags & _FULLWIN)
      && win->_curx == win->_maxx - 1 && win->_cury == win->_maxy - 1)
	return ERR;
  if (_cursvar.echoit && !_cursvar.rawmode) {
	cbreak();
	weset++;
  }
  inp = getchar();
  if (_cursvar.echoit) {
	mvwaddch(curscr, win->_cury + win->_begy,
		 win->_curx + win->_begx, inp);
	waddch(win, inp);
  }
  if (weset) nocbreak();
  return inp;
}
