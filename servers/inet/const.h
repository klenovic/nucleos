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
inet/const.h

Created:	Dec 30, 1991 by Philip Homburg

Copyright 1995 Philip Homburg
*/

#ifndef INET__CONST_H
#define INET__CONST_H

#ifndef DEBUG
#define DEBUG	0
#endif

#ifndef NDEBUG
#define NDEBUG	0
#endif

#define CLOCK_GRAN	1	/* in HZ */

#define where()	printf("%s, %d: ", __FILE__, __LINE__)

#define NW_SUSPEND	SUSPEND
#define NW_WOULDBLOCK	EWOULDBLOCK
#define NW_OK		OK

#define BUF_S		512

#endif /* INET__CONST_H */
