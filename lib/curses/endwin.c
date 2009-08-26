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
#include <termcap.h>

int endwin()
{
  extern char *me;

  curs_set(1);
  poscur(LINES - 1, 0);
  refresh();
  tputs(me, 1, outc);
  delwin(stdscr);
  delwin(curscr);
  delwin(_cursvar.tmpwin);
  resetty();
  return 0;
}
