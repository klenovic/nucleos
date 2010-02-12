/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

#ifndef __NUCLEOS_SYSINFO_H
#define __NUCLEOS_SYSINFO_H

#include <nucleos/endpoint.h>
#include <nucleos/type.h>

int getsysinfo(endpoint_t who, int what, void *where);
ssize_t getsysinfo_up(endpoint_t who, int what, size_t size, void *where);

#define SIU_LOADINFO	1	/* retrieve load info data */
#define SIU_SYSTEMHZ	2	/* retrieve system clock frequency */
#define SIU_IDLETSC	3	/* retrieve cumulative idle timestamp count */

/* Exported system parameters. */

#endif /* __NUCLEOS_SYSINFO_H */
