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
/* Wclear() fills all lines of window 'win' with blanks, and	*/
/* Marks the window to be cleared at next refresh operation.	*/
/****************************************************************/

void wclear(win)
WINDOW *win;
{
  werase(win);
  win->_clear = TRUE;
}				/* wclear */
