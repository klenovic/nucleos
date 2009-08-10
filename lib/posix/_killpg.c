/*
 *  Copyright (C) 2009  Ladislav Klenovic <klenovic@nucleonsoft.com>
 *
 *  This file is part of Nucleos kernel.
 *
 *  Nucleos kernel is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 2 of the License.
 */
#include <lib.h>
#define killpg	_killpg
#define kill	_kill
#include <signal.h>

int killpg(pgrp, sig)
int pgrp;			/* which process group is to be sent the
				 * signal
				 */
int sig;			/* signal number */
{
  return kill(-pgrp, sig);
}
