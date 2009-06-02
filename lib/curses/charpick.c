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

/****************************************************************/
/* Winch(win) returns the character at the current position in	*/
/* Window 'win'.						*/
/****************************************************************/

int winch(win)
WINDOW *win;
{
  return((win->_line[win->_cury][win->_curx]) & 0xff);
}				/* winch */

/****************************************************************/
/* Mvinch() moves the stdscr cursor to a new position, then	*/
/* Returns the character at that position.			*/
/****************************************************************/

int mvinch(y, x)
int y;
int x;
{
  if (wmove(stdscr, y, x) == ERR) return(ERR);
  return((stdscr->_line[stdscr->_cury][stdscr->_curx]) & 0xff);
}

/****************************************************************/
/* Mvwinch() moves the cursor of window 'win' to a new posi-	*/
/* Tion, then returns the character at that position.		*/
/****************************************************************/

int mvwinch(win, y, x)
WINDOW *win;
int y;
int x;
{
  if (wmove(win, y, x) == ERR) return(ERR);
  return((win->_line[win->_cury][win->_curx]) & 0xff);
}
