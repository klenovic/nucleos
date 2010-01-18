/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* Header file for the system service manager server. 
 *
 * Created:
 *    Jul 22, 2005	by Jorrit N. Herder 
 */
#define VERBOSE		   0	/* display diagnostics */

#include <nucleos/types.h>
#include <nucleos/limits.h>
#include <nucleos/errno.h>
#include <nucleos/signal.h>
#include <nucleos/unistd.h>
#include <nucleos/type.h>
#include <nucleos/const.h>
#include <nucleos/com.h>
#include <nucleos/syslib.h>
#include <nucleos/sysutil.h>
#include <nucleos/keymap.h>
#include <nucleos/bitmap.h>
#include <servers/rs/rs.h>

#include <asm/kernel/types.h>
#include <nucleos/timer.h>				/* For priv.h */
#include <kernel/priv.h>

#include <stdlib.h>
#include <stdio.h>
#include <nucleos/string.h>
#include <nucleos/signal.h>

#include "proto.h"
#include "manager.h"

