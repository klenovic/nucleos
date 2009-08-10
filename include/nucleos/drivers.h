/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* This is the master header for all device drivers. It includes some other
 * files and defines the principal constants.
 */
#define _SYSTEM		   1	/* get negative error number in <errno.h> */

/* The following are so basic, all the *.c files get them automatically. */
#include <nucleos/type.h>
#include <nucleos/com.h>
#include <nucleos/dmap.h>
#include <nucleos/callnr.h>
#include <nucleos/types.h>
#include <nucleos/const.h>
#include <nucleos/devio.h>
#include <nucleos/syslib.h>
#include <nucleos/sysutil.h>
#include <nucleos/bitmap.h>

#include <ibm/interrupt.h>	/* IRQ vectors and miscellaneous ports */
#include <ibm/bios.h>		/* BIOS index numbers */
#include <ibm/ports.h>		/* Well-known ports */

#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <nucleos/limits.h>
#include <nucleos/stddef.h>
#include <errno.h>
#include <unistd.h>

