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
/* Werase() fills all lines of window 'win' with blanks and po-	*/
/* Sitions the cursor at home in the scroll region.		*/
/****************************************************************/

void werase(win)
WINDOW *win;
{
  int *end, *start, y, blank;

  blank = ' ' | (win->_attrs & ATR_MSK);

  for (y = win->_regtop; y <= win->_regbottom; y++) {	/* clear all lines */
	start = win->_line[y];
	end = &start[win->_maxx];
	while (start <= end)	/* clear all line */
		*start++ = blank;
	win->_minchng[y] = 0;
	win->_maxchng[y] = win->_maxx;
  }
  win->_cury = win->_regtop;	/* cursor home */
  win->_curx = 0;
}
