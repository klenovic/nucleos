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

_PROTOTYPE( static void ttysetflags, (void) );

static void ttysetflags()
{
  _tty.c_iflag |= ICRNL | IXON;
  _tty.c_oflag |= OPOST | ONLCR;
  _tty.c_lflag |= ECHO | ICANON | IEXTEN | ISIG;

  if (_cursvar.rawmode) {
	_tty.c_iflag &= ~(ICRNL | IXON);
	_tty.c_oflag &= ~(OPOST);
	_tty.c_lflag &= ~(ICANON | IEXTEN | ISIG);
  }
  if (_cursvar.cbrkmode) {
	_tty.c_lflag &= ~(ICANON);
  }
  if (!_cursvar.echoit) {
	_tty.c_lflag &= ~(ECHO | ECHONL);
  }
  if (NONL) {
	_tty.c_iflag &= ~(ICRNL);
	_tty.c_oflag &= ~(ONLCR);
  }
  tcsetattr(0, TCSANOW, &_tty);
}				/* ttysetflags */

void raw()
{
  _cursvar.rawmode = TRUE;
  ttysetflags();
}				/* raw */

void noraw()
{
  _cursvar.rawmode = FALSE;
  ttysetflags();
}				/* noraw */

void echo()
{
  _cursvar.echoit = TRUE;
  ttysetflags();
}

void noecho()
{
  _cursvar.echoit = FALSE;
  ttysetflags();
}

void nl()
{
  NONL = FALSE;
  ttysetflags();
}				/* nl */

void nonl()
{
  NONL = TRUE;
  ttysetflags();
}				/* nonl */

void cbreak()
{
  _cursvar.cbrkmode = TRUE;
  ttysetflags();
}				/* cbreak */

void nocbreak()
{
  _cursvar.cbrkmode = FALSE;
  ttysetflags();
}				/* nocbreak */
