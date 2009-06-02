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
/* Wmove() moves the cursor in window 'win' to position (x,y).	*/
/****************************************************************/

int wmove(win, y, x)
WINDOW *win;
int y;
int x;
{
  if ((x<0) || (x>win->_maxx) || (y<win->_regtop) || (y>win->_regbottom)) 
	return(ERR);
  win->_curx = x;
  win->_cury = y;
  return(OK);
}
