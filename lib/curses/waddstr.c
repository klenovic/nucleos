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
/* Waddstr() inserts string 'str' at the current cursor posi-	*/
/* Tion in window 'win', and takes any actions as dictated by	*/
/* The characters.						*/
/****************************************************************/

int waddstr(win, str)
WINDOW *win;
char *str;
{
  while (*str) {
	if (waddch(win, *str++) == ERR) return(ERR);
  }
  return 0;
}
