/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/*
posix/_cfgetispeed

Created:	June 11, 1993 by Philip Homburg
*/

#include <termios.h>

speed_t _cfgetispeed(const struct termios *termios_p)
{
  return termios_p->c_ispeed;
}
