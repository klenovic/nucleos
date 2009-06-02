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

static bool hasold = FALSE;	/* for remembering old cursor type */
static int oldmode;

/****************************************************************/
/* Idlok() is used to set  flag for using the terminal insert/	*/
/* Delete line capabilities. This is not relevant for the PC	*/
/* Version of curses, and thus nothing is done.			*/
/****************************************************************/
void idlok(win, flag)
WINDOW *win;
bool flag;
{
}

/****************************************************************/
/* Clearok() marks window 'win' to cause screen clearing and	*/
/* Redraw the next time a refresh is done.			*/
/****************************************************************/
void clearok(win, flag)
WINDOW *win;
bool flag;
{
  if (win == curscr)
	_cursvar.tmpwin->_clear = flag;
  else
	win->_clear = flag;
}

/****************************************************************/
/* Leaveok() marks window 'win' to allow the update routines	*/
/* To leave the hardware cursor where it happens to be at the	*/
/* End of update. Usually used in combination with cursoff().	*/
/****************************************************************/

void leaveok(win, flag)
WINDOW *win;
bool flag;
{
  win->_leave = flag;
}

/****************************************************************/
/* Scrollok() marks window 'win' to allow the scrolling region	*/
/* Of it to actually scroll.					*/
/****************************************************************/
void scrollok(win, flag)
WINDOW *win;
bool flag;
{
  win->_scroll = flag;
}

/****************************************************************/
/* Nodelay() marks the window to make character input non-	*/
/* Waiting, i.e. if there is no character to get, -1 will be	*/
/* Returned.							*/
/****************************************************************/
void nodelay(win, flag)
WINDOW *win;
bool flag;
{
  win->_nodelay = flag;
}

/****************************************************************/
/* Keypad() marks window 'win' to use the special keypad mode.	*/
/****************************************************************/
void keypad(win, flag)
WINDOW *win;
bool flag;
{
  win->_keypad = flag;
}

/****************************************************************/
/* Meta() allows use of any alternate character set allowed by	*/
/* The terminal. We always allow this on the PC, so this one	*/
/* Does nothing.						*/
/****************************************************************/
void meta(win, flag)
WINDOW *win;
bool flag;
{
}
