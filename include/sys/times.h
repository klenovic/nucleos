/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The <times.h> header is for time times() system call. */

#ifndef _TIMES_H
#define _TIMES_H

#ifndef _CLOCK_T
#define _CLOCK_T
typedef long clock_t;		/* unit for system accounting */
#endif

struct tms {
  clock_t tms_utime;
  clock_t tms_stime;
  clock_t tms_cutime;
  clock_t tms_cstime;
};

/* Function Prototypes. */
#ifndef _ANSI_H
#include <ansi.h>
#endif

clock_t times(struct tms *_buffer);

#endif /* _TIMES_H */
