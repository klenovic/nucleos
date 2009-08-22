/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */

#define _SYSTEM            1    /* get OK and negative error codes */
#define VERBOSE		   0	/* display diagnostics */

#include <nucleos/types.h>
#include <nucleos/limits.h>
#include <nucleos/errno.h>
#include <signal.h>
#include <unistd.h>

#include <nucleos/callnr.h>
#include <nucleos/type.h>
#include <nucleos/const.h>
#include <nucleos/com.h>
#include <nucleos/syslib.h>
#include <nucleos/sysutil.h>
#include <nucleos/keymap.h>
#include <nucleos/bitmap.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "proto.h"

