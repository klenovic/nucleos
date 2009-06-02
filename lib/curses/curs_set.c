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

extern char *vi, *ve, *vs;

/* Sets cursor visibility to unvisible=0; normal visible=1 or very good
 * visible=2. 
*/
void curs_set(visibility)
int visibility;
{
  switch (visibility) {
      case 0:
	if (vi) tputs(vi, 1, outc);
	break;
      case 1:
	if (ve) tputs(ve, 1, outc);
	break;
      case 2:
	if (vs)
		tputs(vs, 1, outc);
	else if (ve)
		tputs(ve, 1, outc);
  }
}
