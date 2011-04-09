/*
 *  Copyright (C) 2011  Ladislav Klenovic <klenovic@nucleonsoft.com>
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

#ifndef __NUCLEOS_UN_H
#define __NUCLEOS_UN_H

struct sockaddr_un
{
	sa_family_t	sun_family;
	char		sun_path[127];
};

/* Note: UNIX domain sockets are not implemented! */
#endif /* __NUCLEOS_UN_H */
