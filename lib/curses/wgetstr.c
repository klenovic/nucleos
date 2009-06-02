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
/* Wgetstr(win,str) reads in a string (terminated by \n or \r)	*/
/* To the buffer pointed to by 'str', and displays the input	*/
/* In window 'win'. The user's erase and kill characters are	*/
/* Active.							*/
/****************************************************************/

int wgetstr(win, str)
WINDOW *win;
char *str;
{
  while ((*str = wgetch(win)) != ERR && *str != '\n') str++;
  if (*str == ERR) {
	*str = '\0';
	return ERR;
  }
  *str = '\0';
  return OK;
}
