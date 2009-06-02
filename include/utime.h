/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* The <utime.h> header is used for the utime() system call. */

#ifndef _UTIME_H
#define _UTIME_H

#ifndef _TYPES_H
#include <sys/types.h>
#endif

struct utimbuf {
  time_t actime;		/* access time */
  time_t modtime;		/* modification time */
};

/* Function Prototypes. */
_PROTOTYPE( int utime, (const char *_path, const struct utimbuf *_times)     );

#endif /* _UTIME_H */
