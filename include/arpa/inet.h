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
arpa/inet.h
*/

#ifndef _ARPA__INET_H
#define _ARPA__INET_H

#include <nucleos/types.h>

/* Open Group Base Specifications Issue 6 (not complete): */

#ifndef _IN_ADDR_T
#define _IN_ADDR_T
/* Has to match corresponding declaration in <netinet/in.h> */
typedef uint32_t	in_addr_t;
#endif /* _IN_ADDR_T */

#ifndef _STRUCT_IN_ADDR
#define _STRUCT_IN_ADDR
/* Has to match corresponding declaration in <netinet/in.h> */
struct in_addr
{
	in_addr_t	s_addr;
};
#endif

uint32_t htonl(uint32_t _hostval);
uint16_t htons(uint16_t _hostval);
char *inet_ntoa(struct in_addr _in);
uint32_t ntohl(uint32_t _netval);
uint16_t ntohs(uint16_t _netval);

/* Additional functions */
int inet_aton(const char *_cp, struct in_addr *_pin);

#endif /* _ARPA__INET_H */
