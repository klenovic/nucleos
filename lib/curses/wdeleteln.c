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
/* Wdeleteln() deletes the line at the window cursor, and the	*/
/* Lines below it are shifted up, inserting a blank line at	*/
/* The bottom of the window.					*/
/****************************************************************/

int wdeleteln(win)
WINDOW *win;
{
  int *end, *temp, y, blank;

  blank = ' ' | (win->_attrs & ATR_MSK);

  temp = win->_line[win->_cury];
  for (y = win->_cury; y < win->_regbottom; y++) {
	win->_line[y] = win->_line[y + 1];
	win->_minchng[y] = 0;
	win->_maxchng[y] = win->_maxx;
  }
  win->_minchng[y] = 0;
  win->_maxchng[y] = win->_maxx;
  win->_line[win->_regbottom] = temp;
  for (end = &(temp[win->_maxx]); temp <= end;) *temp++ = blank;
  return 0;
}
