/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* Header file for the system information server. 
 *
 * Created:
 *    Jul 13, 2004	by Jorrit N. Herder 
 */
#include <nucleos/types.h>
#include <nucleos/limits.h>
#include <nucleos/errno.h>

#include <nucleos/unistd.h>
#include <nucleos/type.h>
#include <nucleos/const.h>
#include <nucleos/com.h>
#include <nucleos/syslib.h>
#include <nucleos/sysutil.h>
#include <nucleos/keymap.h>
#include <nucleos/bitmap.h>

#include <stdlib.h>
#include <stdio.h>
#include <nucleos/string.h>
#include <nucleos/signal.h>

#include <asm/kernel/types.h>
#include "proto.h"
#include "glo.h"

