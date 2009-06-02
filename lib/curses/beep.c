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

extern char *bl, *vb;

/* Beep() sounds the terminal bell. */
void beep()
{
  if (bl)
	tputs(bl, 1, outc);
  else if (vb)
	tputs(vb, 1, outc);
}
