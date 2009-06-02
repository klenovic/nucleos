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
/* Mvcur(oldy,oldx,newy,newx) the display cursor to <newy,newx>	*/
/****************************************************************/

int mvcur(oldy, oldx, newy, newx)
int oldy;
int oldx;
int newy;
int newx;
{
  if ((newy >= LINES) || (newx >= COLS) || (newy < 0) || (newx < 0))
	return(ERR);
  poscur(newy, newx);
  _cursvar.cursrow = newy;
  _cursvar.curscol = newx;
  return(OK);
}
