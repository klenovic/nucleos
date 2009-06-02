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
sys/un.h
*/

/* Open Group Base Specifications Issue 6 */

#ifndef _SA_FAMILY_T
#define _SA_FAMILY_T
/* Should match corresponding typedef in <sys/socket.h> */
typedef uint8_t		sa_family_t;
#endif /* _SA_FAMILY_T */

struct sockaddr_un
{
	sa_family_t	sun_family;
	char		sun_path[127];
};

/* Note: UNIX domain sockets are not implemented! */
