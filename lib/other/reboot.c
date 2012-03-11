/*
 *  Copyright (C) 2012  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
/* reboot.c - Systemcall interface to mm/signal.c::do_reboot()

   author: Edvard Tuinder  v892231@si.hhs.NL
 */

#include <nucleos/lib.h>
#include <nucleos/unistd.h>
#include <stdarg.h>

int reboot(int how)
{
	return INLINE_SYSCALL(reboot, 1, RBT_RESET);
}
