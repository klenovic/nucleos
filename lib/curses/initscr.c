/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* initscr.c - initialize the curses library */

#include <stdlib.h>
#include <curses.h>
#include "curspriv.h"

WINDOW *initscr()
{
  char *term;

  if ((term = getenv("TERM")) == NULL) return NULL;
  setterm(term);
  gettmode();
  if ((_cursvar.tmpwin = newwin(LINES, COLS, 0, 0)) == NULL) return NULL;
  if ((curscr = newwin(LINES, COLS, 0, 0)) == NULL) return NULL;
  if ((stdscr = newwin(LINES, COLS, 0, 0)) == NULL) return NULL;
  clearok(curscr, TRUE);
  return(stdscr);
}
