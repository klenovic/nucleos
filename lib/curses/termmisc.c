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

/* Static variables or saving terminal modes */

int fixterm()
{
  return 0;
}				/* fixterm */

int resetterm()
{
  return 0;
}

int saveoldterm()
{
  return 0;
}				/* saveoldterm */

int saveterm()
{
  return 0;
}				/* saveterm */

int baudrate()
{
  return(19200);
}				/* baudrate */

/****************************************************************/
/* Erasechar(), killchar() returns std MSDOS erase chars.	*/
/****************************************************************/

int erasechar()
{
  return(_DCCHAR);		/* character delete char */
}				/* erasechar */

int killchar()
{
  return(_DLCHAR);		/* line delete char */
}				/* killchar */

/****************************************************************/
/* Savetty() and resetty() saves and restores the terminal I/O	*/
/* Settings.							*/
/****************************************************************/

int savetty()
{
  return 0;
}				/* savetty */

/****************************************************************/
/* Setupterm() sets up the terminal. On a PC, it is always suc-	*/
/* Cessful, and returns 1.					*/
/****************************************************************/

int setupterm()
{
  return(1);
}				/* setupterm */
