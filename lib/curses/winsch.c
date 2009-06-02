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
#include "curspriv.h"

/* Winsch() inserts character 'c' at the cursor position in
   window 'win'. The cursor is advanced.
*/

int winsch(win, c)
WINDOW *win;
char c;
{
  int *temp1;
  int *temp2;
  int *end;
  int x = win->_curx;
  int y = win->_cury;
  int maxx = win->_maxx;

  if ((c < ' ') && (c == '\n' || c == '\r' || c == '\t' || c == '\b'))
	return(waddch(win, c));
  end = &win->_line[y][x];
  temp1 = &win->_line[y][maxx];
  temp2 = temp1 - 1;
  if (c < ' ')			/* if CTRL-char make space for 2 */
	temp2--;
  while (temp1 > end) *temp1-- = *temp2--;
  win->_maxchng[y] = maxx;
  if ((win->_minchng[y] == _NO_CHANGE) || (win->_minchng[y] > x))
	win->_minchng[y] = x;
  return(waddch(win, c));	/* fixes CTRL-chars too */
}				/* winsch */
